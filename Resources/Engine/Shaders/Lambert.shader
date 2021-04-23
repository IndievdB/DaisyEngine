// ============================================
// Begin Vertex Shader
// ============================================

#version 430
in vec3 aPos : POSITION;
in vec2 aTexCoords : TEXCOORD;
in vec3 aNormal : NORMAL;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 ViewPos;
out vec2 TexCoords;
out vec3 VSNormal;
out vec3 WSNormal;
out vec3 WorldPos;

void main()
{
	WorldPos = vec3(model * vec4(aPos, 1.0));

	vec4 viewPos = view * model * vec4(aPos, 1.0);
	ViewPos = viewPos.xyz;
	TexCoords = aTexCoords;
	
	mat3 normalMatrix = transpose(inverse(mat3(view * model)));
	VSNormal = normalMatrix * (vec4(aNormal, 1.0)).xyz;
	WSNormal = mat3(transpose(inverse(model))) * aNormal;
	
	gl_Position = projection * viewPos;
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 430

uniform float nearPlane;
uniform float farPlane;
uniform float screenWidth;
uniform float screenHeight;
uniform mat4 view;
uniform sampler2D mainTex;
uniform samplerCube pointShadowMap;
uniform samplerCubeArray shadowCubeMapArray;

in vec3 WorldPos;
in vec3 ViewPos;
in vec2 TexCoords;
in vec3 VSNormal;
in vec3 WSNormal;

out vec4 FragColor;

uniform vec2 clusterScreenSpaceSize;

const int numLights = 256;
const int numClusters = 512;

struct Cluster
{
	vec4 minBounds;
	vec4 maxBounds;
};

struct PointLight
{
	vec4 position;
	vec4 color;
	float radius;
	float intensity;
	bool castsShadows;
	float pl_padding[1];
};

struct SpotLight
{
	vec4 position;
	vec4 direction;
	vec4 color;
	float intensity;
	float cutOff;
	float range;
	float sl_padding[1];
};

struct AmbientLight
{
	vec4 color;
	float intensity;
	float al_padding[3];
};

struct DirectionalLight
{
	vec4 direction;
	vec4 color;
	float intensity;
	float dl_padding[3];
};

layout(std430, binding = 0) buffer ClusterBuffer
{
	Cluster clusters[];
};

layout(std430, binding = 1) buffer PointLightBuffer
{
	PointLight pointLights[numLights];
	int clusterPointLightCount[numClusters];
	int clusterPointLightIndices[numClusters][numLights];
};

layout(std430, binding = 2) buffer SpotLightBuffer
{
	SpotLight spotLights[numLights];
	int clusterSpotLightCount[numClusters];
	int clusterSpotLightIndices[numClusters][numLights];
};

layout(std430, binding = 3) buffer GlobalLightBuffer
{
	AmbientLight ambientLights[numLights];
	DirectionalLight directionalLights[numLights];
	int numberOfAmbientLights;
	int numberOfDirectionalLights;
	int glb_padding[2];
};

float Screen2EyeDepth(float depth, float near, float far)
{
	float ndc = 2.0 * depth - 1.0;
	float eye = 2.0 * far * near / (far + near + ndc * (near - far));
	return eye;
}

uint GetClusterZIndex(float screenDepth)
{
	float scale = float(8) / log(farPlane / nearPlane);
	float bias = -(float(8) * log(nearPlane) / log(farPlane / nearPlane));

	float eyeDepth = Screen2EyeDepth(screenDepth, nearPlane, farPlane);
	uint zIndex = uint(max(log(eyeDepth) * scale + bias, 0.0));
	return zIndex;
}

uint GetClusterIndex(vec4 fragCoord)
{
	uint zIndex = GetClusterZIndex(fragCoord.z);
	uvec3 indices = uvec3(uvec2(fragCoord.xy / clusterScreenSpaceSize), zIndex);
	uint cluster = (8 * 8) * indices.z + 8 * indices.y + indices.x;
	return cluster;
}

vec3 gridSamplingDisk[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float ShadowCalculationPoint(vec3 fragPos, vec3 lightPos, float lightRadius, int lightIndex)
{
	vec3 fragToLight = fragPos - lightPos;
	float currentDepth = length(fragToLight);

	float shadow = 0.0;
	float bias = 0.15;
	int samples = 20;
	float viewDistance = length(ViewPos - fragPos);
	float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;

	for (int i = 0; i < samples; ++i)
	{
		vec3 index = fragToLight + gridSamplingDisk[i] * diskRadius;
		float closestDepth = texture(shadowCubeMapArray, vec4(index, lightIndex)).r;
		//float closestDepth = texture(shadowCubeMapArray, vec4(index, 0)).r;
		//float closestDepth = texture(pointShadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
		closestDepth *= lightRadius;   // undo mapping [0;1]
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}

	shadow /= float(samples);

	return shadow;
}

vec3 GetLighting(vec4 fragCoord)
{
	uint clusterIndex = GetClusterIndex(gl_FragCoord);
	Cluster cluster = clusters[clusterIndex];
	vec3 lighting = vec3(0,0,0);

	// Ambient Lights
	for (int i = 0; i < numberOfAmbientLights; i++)
	{
		AmbientLight light = ambientLights[i];

		vec3 diffuse = light.color.rgb;
		diffuse *= light.intensity;
		lighting.rgb += diffuse;
	}

	// Directional Lights
	for (int i = 0; i < numberOfDirectionalLights; i++)
	{
		DirectionalLight light = directionalLights[i];

		vec3 lightDir = normalize(-light.direction.xyz);
		float diff = max(dot(WSNormal, lightDir), 0.0);
		vec3 diffuse = light.color.rgb * diff;
		diffuse *= light.intensity;

		lighting.rgb += diffuse;
	}

	// Point Lights
	for (int i=0; i < clusterPointLightCount[clusterIndex]; i++)
	{
		int pointLightIndex = clusterPointLightIndices[clusterIndex][i];
		PointLight light = pointLights[pointLightIndex];

		float dist = length(light.position.xyz - WorldPos);

		if (dist < light.radius)
		{
			vec3 lightDir = normalize(light.position.xyz - WorldPos);
			float diff = max(dot(WSNormal, lightDir), 0.0);
			vec3 diffuse = diff * light.color.rgb;

			float attenuation = clamp(1.0 - dist / light.radius, 0.0, 1.0); attenuation *= attenuation;
			attenuation = clamp(attenuation * light.intensity, 0.0, 1.0);
			diffuse *= attenuation;

			if (light.castsShadows)
			{
				float shadow = ShadowCalculationPoint(WorldPos, light.position.xyz, light.radius, pointLightIndex);
				diffuse *= (1.0 - shadow);
			}
			
			lighting.rgb += diffuse;
		}
	}

	// Spot Lights
	for (int i = 0; i < clusterSpotLightCount[clusterIndex]; i++)
	{
		int spotLightIndex = clusterSpotLightIndices[clusterIndex][i];
		SpotLight light = spotLights[spotLightIndex];

		float dist = length(light.position.xyz - WorldPos);

		if (dist > light.range)
			continue;

		vec3 lightDir = normalize(light.position.xyz - WorldPos);
		float theta = dot(lightDir, normalize(-light.direction.xyz));

		if (theta > cos(radians(light.cutOff)))
		{
			float diff = max(dot(WSNormal, lightDir), 0.0);
			vec3 diffuse = light.color.rgb * diff;
			diffuse *= light.intensity;
			lighting += diffuse;
		}
	}

	return lighting;
}

void main(void)
{
	vec4 albedoColor = texture2D(mainTex, TexCoords);
	vec3 lighting = GetLighting(gl_FragCoord);

	FragColor = vec4(lighting * albedoColor.rgb, 1);
}
