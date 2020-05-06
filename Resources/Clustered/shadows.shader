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
uniform mat4 lightSpaceMatrix;

out vec3 ViewPos;
out vec3 WorldPos;
out vec4 WorldPosLightSpace;
out vec2 TexCoords;
out vec3 Normal;

void main()
{
	WorldPos = vec3(model * vec4(aPos, 1.0));
	WorldPosLightSpace = lightSpaceMatrix * vec4(WorldPos, 1.0);

	vec4 viewPos = view * model * vec4(aPos, 1.0);
	ViewPos = viewPos.xyz;
	TexCoords = aTexCoords;
	
	Normal = mat3(transpose(inverse(model))) * aNormal;
	
	gl_Position = projection * viewPos;
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 430

const int numLights = 512;
const vec3 tilesOnAxes = vec3(8, 8, 8);
const int numTiles = int(tilesOnAxes.x * tilesOnAxes.y * tilesOnAxes.z);

int GetTileIndex(const int xIndex, const int yIndex, const int zIndex)
{
	return xIndex + int(tilesOnAxes.x) * (yIndex + int(tilesOnAxes.y) * zIndex);
}

struct LightData
{
	vec4 pos4;
	vec4 lightColour;
	float lightRadius;
	float intensity;

	float fpadding[2];
};

struct Tile
{
	float x;
	float y;
	float z;
	float width;
	float height;
	float length;

	float _padding[6];
};

layout (std430, binding = 1) buffer LightDataBuffer
{
	LightData lightData[];
};

layout (std430, binding = 3) buffer TileLightsBuffer
{
	int lightIndexes[numTiles];
	int tileLights[][numLights];
};

layout(binding = 0) uniform atomic_uint count;

uniform float nearPlane;
uniform float farPlane;
uniform float screenWidth;
uniform float screenHeight;
uniform float ambientLighting;
uniform mat4 view;
uniform sampler2D mainTex;
uniform sampler2D shadowMap;
uniform vec3 camPos;


in vec3 ViewPos;
in vec3 WorldPos;
in vec4 WorldPosLightSpace;
in vec2 TexCoords;
in vec3 Normal;

out vec4 FragColor;



struct DirectionalLight
{
	vec3 direction;
	vec3 color;
};

uniform DirectionalLight directionalLight;

void AddDirectionalLight(DirectionalLight light, vec4 albedoCol, vec3 normal, inout vec4 lightResult)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.color * diff * albedoCol.rgb;
	lightResult.rgb += diffuse;
}

float ShadowCalculation(DirectionalLight light, vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	if (projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

void AddBPLighting(vec3 position, vec3 normal, vec4 albedoCol, int lightIndex, inout vec4 lightResult)
{
	vec3 lightPosition = lightData[lightIndex].pos4.xyz;
	float dist = length(lightPosition - position);

	if (dist <= lightData[lightIndex].lightRadius)
	{
		vec3 lightDir = normalize(lightPosition - position);
		vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedoCol.rgb * lightData[lightIndex].lightColour.rgb;
		float attenuation = 1.0 - clamp(dist / lightData[lightIndex].lightRadius, 0.0, 1.0);

		attenuation *= lightData[lightIndex].intensity;
		diffuse *= attenuation;

		lightResult.rgb += diffuse;
	}
}

void main(void)
{
	vec2 screenSpacePosition = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight);
	vec3 normal = normalize(Normal);

	vec4 col = texture2D(mainTex, TexCoords);
	if (col.a < 0.1f)
	{
		discard;
	}

	vec4 albedoCol = vec4(col.rgb, 1);

	float zCoord = abs(ViewPos.z - nearPlane) / (farPlane - nearPlane);

	int xIndex = int(screenSpacePosition.x * (tilesOnAxes.x));
	int yIndex = int(screenSpacePosition.y * (tilesOnAxes.y));
	int zIndex = int(zCoord * (tilesOnAxes.z));

	int tile = GetTileIndex(xIndex, yIndex, zIndex);

	vec4 lightResult = vec4(0.0, 0.0, 0.0, 1.0);
	for (int j = 0; j < lightIndexes[tile]; j++)
	{
		int lightIndex = tileLights[tile][j];
		AddBPLighting(WorldPos, normal, albedoCol, lightIndex, lightResult);
	}

	AddDirectionalLight(directionalLight, albedoCol, normal, lightResult);

	float shadow = ShadowCalculation(directionalLight, WorldPosLightSpace);
	lightResult.rgb *= (1.0 - shadow);
	lightResult.rgb += albedoCol.rgb * ambientLighting;
	lightResult.a = albedoCol.a;

	FragColor = lightResult;
}