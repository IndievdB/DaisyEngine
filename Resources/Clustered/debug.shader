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

struct SpotLightData
{
	vec4 lightPosition;
	vec4 lightDirection;
	vec4 lightColor;
	float intensity;
	float cutOff;
	float range;
	float sl_padding[1];
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

layout(binding = 0) uniform atomic_uint count;
layout(binding = 0, offset = 4) uniform atomic_uint spotLightCount;

layout (std430, binding = 1) buffer LightDataBuffer
{
	LightData lightData[];
};

layout(std430, binding = 6) buffer SpotLightDataBuffer
{
	SpotLightData spotLightData[];
};

layout (std430, binding = 2) buffer TileLightsBuffer
{
	int lightIndexes[numTiles];
	int tileLights[numTiles][numLights];

	int spotLightIndexes[numTiles];
	int tileSpotLights[numTiles][numLights];
};

/*layout(std430, binding = 5) buffer AmbientDirectionalLightData
{
	vec4 ambientLightsColors[numLights];
	float ambientLightsIntensities[numLights];

	vec4 directionalLightsDirections[numLights];
	vec4 directionalLightsColors[numLights];
	float directionalLightsIntensities[numLights];

	int numAmbientLights;
	int numDirectionalLights;
	int adld_padding[2];
};*/

/*layout(std430, binding = 5) buffer DirectionalLightDataBuffer
{
	vec4 directionalLightsDirections[numLights];
	vec4 directionalLightsColors[numLights];
	float directionalLightsIntensities[numLights];
	int numDirectionalLights;
	int _padding[3];
};*/

/*layout(std430, binding = 6) buffer SpotLightDataBuffer
{
	vec4 spotLightPositions[numLights];
	vec4 spotLightDirections[numLights];
	vec4 spotLightColors[numLights];

	float spotLightIntensities[numLights];
	float spotLightCutOffs[numLights];

	int numSpotLights;
	int _spotLightDataBufferPadding[3];
};*/

/*layout(std430, binding = 7) buffer AmbientLightDataBuffer
{
	vec4 ambientLightsColors[numLights];
	float ambientLightsIntensities[numLights];
	int numAmbientLights;
	int _ambientLightDataBufferPadding[3];
};*/

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

void AddBPLighting(vec3 position, vec3 normal, vec4 albedoCol, int lightIndex, inout vec4 lightResult)
{
	vec3 lightPosition = lightData[lightIndex].pos4.xyz;
	float dist = length(lightPosition - position);

	if (dist <= lightData[lightIndex].lightRadius)
	{
		lightResult.rgb = vec3(1, 1, 1);
		/*vec3 lightDir = normalize(lightPosition - position);
		vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedoCol.rgb * lightData[lightIndex].lightColour.rgb;
		float attenuation = 1.0 - clamp(dist / lightData[lightIndex].lightRadius, 0.0, 1.0);

		attenuation *= lightData[lightIndex].intensity;
		diffuse *= attenuation;

		//float shadow = ShadowCalculationPoint(WorldPos, lightPosition, lightData[lightIndex].lightRadius);
		//diffuse *= (1.0 - shadow);

		lightResult.rgb += diffuse;*/
	}
}

/*void AddDirectionalLight(vec3 normal, vec4 albedoCol, int lightIndex, inout vec4 lightResult)
{
	vec3 lightDir = normalize(-directionalLightsDirections[lightIndex].xyz);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = directionalLightsColors[lightIndex].xyz * diff * albedoCol.rgb;
	diffuse *= directionalLightsIntensities[lightIndex];

	//float shadow = ShadowCalculation(light, WorldPosLightSpace);
	//diffuse *= (1.0 - shadow);

	lightResult.rgb += diffuse;
}*/

void AddSpotLight(vec3 position, vec3 normal, vec4 albedoCol, int lightIndex, inout vec4 lightResult)
{
	vec3 lightPosition = spotLightData[lightIndex].lightPosition.xyz;
	float dist = length(lightPosition - position);

	if (dist > spotLightData[lightIndex].range)
	{
		return;
	}
	
	vec3 lightDir = normalize(lightPosition - position);
	float theta = dot(lightDir, normalize(-spotLightData[lightIndex].lightDirection.xyz));

	if (theta > cos(radians(spotLightData[lightIndex].cutOff)))
	{
		lightResult.rgb += vec3(1,1,1);
	}
}

/*void AddSpotLight(vec3 position, vec3 normal, vec4 albedoCol, int lightIndex, inout vec4 lightResult)
{
	vec3 lightPosition = spotLightPositions[lightIndex].xyz;
	vec3 lightDir = normalize(lightPosition - position);
	float theta = dot(lightDir, normalize(-spotLightDirections[lightIndex].xyz));

	if (theta > cos(radians(spotLightCutOffs[lightIndex])))
	{
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = spotLightColors[lightIndex].xyz * diff * albedoCol.rgb;
		diffuse *= spotLightIntensities[lightIndex];

		lightResult.rgb += diffuse;
	}
}*/

/*void AddAmbientLight(vec4 albedoCol, int lightIndex, inout vec4 lightResult)
{
	vec3 diffuse = ambientLightsColors[lightIndex].rgb * albedoCol.rgb;
	diffuse *= ambientLightsIntensities[lightIndex];
	lightResult.rgb += diffuse;
	//lightResult.rgb += vec3(0.1,0.1,0.1);
}*/

void main(void)
{
	vec2 screenSpacePosition = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight);
	float zCoord = abs(FragPos.z - nearPlane) / (farPlane - nearPlane);
	int xIndex = int(screenSpacePosition.x * (tilesOnAxes.x));
	int yIndex = int(screenSpacePosition.y * (tilesOnAxes.y));
	int zIndex = int(zCoord * (tilesOnAxes.z));
	int tile = GetTileIndex(xIndex, yIndex, zIndex);

	float outputColor = float(lightIndexes[tile])/10.0;
	//float outputColor = 0;

	vec4 lightResult = vec4(outputColor, outputColor, outputColor, 1.0);

	vec3 WSnormal = normalize(WSNormal);
	vec4 albedoCol = vec4(1,1,1, 1);

	for (int j = 0; j < lightIndexes[tile]; j++)
	{
		int lightIndex = tileLights[tile][j];
		AddBPLighting(WorldPos, WSnormal, albedoCol, lightIndex, lightResult);
	}

	/*uint numSpotLights = atomicCounter(spotLightCount);
	for (int j = 0; j < numSpotLights; j++)
	{
		AddSpotLight(WorldPos, WSnormal, albedoCol, j, lightResult);
	}*/

	for (int j = 0; j < spotLightIndexes[tile]; j++)
	{
		lightResult.rgb = vec3(0.1, 0.1, 0.1);
		int lightIndex = tileSpotLights[tile][j];
		AddSpotLight(WorldPos, WSnormal, albedoCol, lightIndex, lightResult);
	}

	FragColor = vec4(lightResult);

	//

	/*uint spotLightsOnScreen = atomicCounter(spotLightCount);
	uint lightsOnScreen = atomicCounter(count);
	FragColor = vec4(spotLightsOnScreen, lightsOnScreen, 0, 1);*/

	//FragColor = vec4(0, 0, 0, 1);

	/*vec2 screenSpacePosition = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight);

	vec3 VSnormal = normalize(VSNormal);
	vec3 WSnormal = normalize(WSNormal);

	vec4 col = texture2D(mainTex, TexCoords);

	if (col.a < 0.1f)
	{
		discard;
	}

	vec4 albedoCol = vec4(col.rgb, 1);

	float zCoord = abs(FragPos.z - nearPlane) / (farPlane - nearPlane);

	int xIndex = int(screenSpacePosition.x * (tilesOnAxes.x));
	int yIndex = int(screenSpacePosition.y * (tilesOnAxes.y));
	int zIndex = int(zCoord * (tilesOnAxes.z));

	int tile = GetTileIndex(xIndex, yIndex, zIndex);

	vec4 lightResult = vec4(0.0, 0.0, 0.0, 1.0);

	for (int j = 0; j < lightIndexes[tile]; j++)
	{
		int lightIndex = tileLights[tile][j];
		AddBPLighting(WorldPos, WSnormal, albedoCol, lightIndex, lightResult);
	}

	for (int j = 0; j < numDirectionalLights; j++)
	{
		AddDirectionalLight(WSnormal, albedoCol, j, lightResult);
	}

	//for (int j = 0; j < numSpotLights; j++)
	//{
	//	AddSpotLight(WorldPos, WSnormal, albedoCol, j, lightResult);
	//}

	for (int j = 0; j < numAmbientLights; j++)
	{
		AddAmbientLight(albedoCol, j, lightResult);
	}

	lightResult.a = albedoCol.a;
	FragColor = lightResult;*/
}