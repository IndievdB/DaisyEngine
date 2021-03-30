#version 430 core

#include Resources/Clustered/configuration.glsl

uniform float nearPlane;
uniform float farPlane;
uniform int numZTiles;
uniform int numLightsInFrustum;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform vec4 cameraPosition;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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

//faces - xyz is normal, w is distance from origin
struct GridCube
{
	vec4 planes[6];
};

/*struct TileBounds
{
	float left, right, top, bottom, front, back;

	float padding[2];
};*/

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

//Shared with lighting shader

layout(binding = 0) uniform atomic_uint count;
layout(binding = 0, offset = 4) uniform atomic_uint spotLightCount;

layout(std430, binding = 1) buffer LightDataBuffer
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

layout(std430, binding = 3) buffer ScreenSpaceDataBuffer
{
	float indexes[numLights];
	float zRadius[numLights];
	vec4 NDCCoords[numLights];

	float indexesSL[numLights];
};

/*layout(std430, binding = 4) buffer TileBoundsBuffer
{
	TileBounds tileBounds[];
};*/

layout(std430, binding = 4) buffer GridCubesBuffer
{
	GridCube gridCubes[];
};

#include Resources/Clustered/collisionFunctions.glsl

void main()
{
	int xIndex = int(gl_GlobalInvocationID.x);
	int yIndex = int(gl_GlobalInvocationID.y);
	int zIndex = int(gl_GlobalInvocationID.z);

	uint index = uint(xIndex + int(tilesOnAxes.x) * (yIndex + int(tilesOnAxes.y) * zIndex));

	int intersections = 0;

	uint lightsOnScreen = atomicCounter(count);
	for (int i = 0; i < lightsOnScreen; i++)
	{
		int lightIndex = int(indexes[i]);
		//TileBounds bounds = tileBounds[index];
		//GridCube cube = gridCubes[index];

		vec4 worldPos = lightData[lightIndex].pos4;
		vec4 viewPos = viewMatrix * lightData[lightIndex].pos4;

		if (PointlightInCube(gridCubes[index].planes, viewPos))
		//if (PointlightInCube(gridCubes[index].planes, NDCCoords[i]))
		//if (true)
		//if (PointLightInCluster(NDCCoords[i], zRadius[i], bounds.left, bounds.right, bounds.bottom, bounds.top, bounds.front, bounds.back))
		//if (CollideTest2(bounds.left, bounds.right, bounds.bottom, bounds.top, bounds.front, bounds.back, NDCCoords[i], zRadius[i]))
		{
			tileLights[index][intersections] = lightIndex;
			++intersections;
		}
	}

	lightIndexes[index] = intersections;

	// spot lights

	/*int spotLightIntersections = 0;

	uint spotLightsOnScreen = atomicCounter(spotLightCount);
	for (int i = 0; i < spotLightsOnScreen; i++)
	{
		int lightIndex = int(indexesSL[i]);
		//TileBounds bounds = tileBounds[index];

		if (true) // spot light in tile
		{
			tileSpotLights[index][spotLightIntersections] = lightIndex;
			++spotLightIntersections;
		}
	}

	spotLightIndexes[index] = spotLightIntersections;*/
}
