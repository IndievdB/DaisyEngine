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

out vec3 FragPos;
out vec2 TexCoords;
out vec3 VSNormal;
out vec3 WSNormal;
out vec3 WorldPos;

void main()
{
	WorldPos = vec3(model * vec4(aPos, 1.0));

	vec4 viewPos = view * model * vec4(aPos, 1.0);
	FragPos = viewPos.xyz;
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

in vec3 WorldPos;
in vec3 FragPos;
in vec2 TexCoords;
in vec3 VSNormal;
in vec3 WSNormal;

out vec4 FragColor;

uniform vec2 clusterScreenSpaceSize;

const int numLights = 512;
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
	float pl_padding[2];
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

void main(void)
{
	uint clusterIndex = GetClusterIndex(gl_FragCoord);
	Cluster cluster = clusters[clusterIndex];

	vec4 albedoColor = texture2D(mainTex, TexCoords);
	vec3 lighting = vec3(0, 0, 0);

	// Ambient Light

	if (true)
	{
		vec3 diffuse = vec3(1,1,1) * albedoColor.rgb;
		diffuse *= 0.1f;
		lighting.rgb += diffuse;
	}

	// Directional Light

	// Point Light

	/*for (int i=0; i < clusterPointLightCount[clusterIndex]; i++)
	{
		int pointLightIndex = clusterPointLightIndices[clusterIndex][i];
		PointLight light = pointLights[pointLightIndex];

		float dist = length(light.position.xyz - WorldPos);
		bool isLit = dist < light.radius;

		lighting += vec3(0.1, 0.1, 0.1);

		if (isLit)
			lighting += vec3(1, 0, 0);
	}*/

	// Spot Light

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
			vec3 diffuse = light.color.rgb * diff * albedoColor.rgb;
			diffuse *= light.intensity;
			lighting += diffuse;
		}
	}

	FragColor = vec4(lighting, 1);
}
