#version 430 core

#include Resources/Clustered/configuration.glsl

uniform float nearPlane;
uniform float farPlane;
uniform mat4 projectionMatrix;
uniform mat4 projView;
uniform mat4 viewMatrix;
uniform int lightsInScene;
uniform int spotLightsInScene;
uniform float aspectRatio;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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

layout(binding = 0) uniform atomic_uint count;
layout(binding = 0, offset = 4) uniform atomic_uint spotLightCount;
//layout(binding = 5) uniform atomic_uint spotLightCount;

//Shared with lighting shader
layout(std430, binding = 1) buffer LightDataBuffer
{
	LightData lightData[];
};

layout(std430, binding = 6) buffer SpotLightDataBuffer
{
	SpotLightData spotLightData[];
};

layout(std430, binding = 3) buffer ScreenSpaceDataBuffer
{
	float indexes[numLights];
	float zRadius[numLights]; 
	vec4 NDCCoords[numLights];

	float indexesSL[numLights];
};

#include Resources/Clustered/collisionFunctions.glsl

void main()
{
	//spotLightCount = spotLightsInScene;
	//atomicCounterAdd(spotLightCount, spotLightsInScene);

	int xIndex = int(gl_GlobalInvocationID.x);
	int yIndex = int(gl_GlobalInvocationID.y);
	int zIndex = int(gl_GlobalInvocationID.z);

	int id = xIndex + int(gl_NumWorkGroups.x) * (yIndex + int(gl_NumWorkGroups.y) * zIndex);

	if (id < spotLightsInScene)
	{
		vec4 frustum[6];
		FrustumFromMatrix(projView, frustum);
		bool colliding = SpotlightInFrustrum(spotLightData[0], frustum);

		//if (SpotlightBehindPlane(spotLightData[0], vec4(0,1,0,0)))
		if (colliding)
		{
			uint currentLightCount = atomicCounterIncrement(spotLightCount);
			indexesSL[currentLightCount] = id;
		}
	}

	if (id < lightsInScene)
	{
		vec4 worldLight = vec4(lightData[id].pos4.xyz, 1.0f);

		vec4 frustum[6];
		FrustumFromMatrix(projView, frustum);
		//bool colliding = PointlightInFrustrum(frustum, vec4(worldLight.xyz, lightData[id].lightRadius));
		bool colliding = true;

		//If light affects any clusters on screen, send to next shader for allocation, 
		//else cull.
		if (colliding)
		{
			/*vec4 projViewPos = projView * worldLight;
			vec4 viewPos = viewMatrix * worldLight;
			float zCoord = -(viewPos.z - nearPlane) / farPlane + nearPlane;
			float w = 1.0f / projViewPos.w;
			float fovy = 2 * atan(1/projectionMatrix[1].y);
			float radius = (lightData[id].lightRadius / (tan((fovy/2.0f)) * abs(viewPos.z)));
			vec4 S = viewMatrix * (worldLight + vec4(lightData[id].lightRadius,0,0,0));
			float viewRadius = distance (viewPos.xyz, S.xyz);
			vec4 S1 = projectionMatrix * (viewPos + vec4(viewRadius,0,0,0));
			vec4 clipPos = vec4(projViewPos.x * w, projViewPos.y * w, zCoord, radius);
			uint currentLightCount = atomicCounterIncrement(count);
			NDCCoords[currentLightCount] = clipPos;
			zRadius[currentLightCount] = lightData[id].lightRadius / (farPlane - nearPlane);
			indexes[currentLightCount] = id;*/

			vec4 projViewPos = projView * worldLight;
			vec4 viewPos = viewMatrix * worldLight;
			float zCoord = abs(viewPos.z - nearPlane) / farPlane + nearPlane;
			float w = 1.0f / projViewPos.w;
			//float radius = lightData[id].lightRadius * w;

			float fovy = 2 * atan(1.0/projectionMatrix[1][1]);
			float D = length(viewPos.xyz);
			float radius = (lightData[id].lightRadius / (tan(fovy/2.0f) * D)) * aspectRatio;

			vec4 clipPos = vec4(projViewPos.x * w, projViewPos.y * w, zCoord, radius);
			uint currentLightCount = atomicCounterIncrement(count);
			NDCCoords[currentLightCount] = clipPos;
			indexes[currentLightCount] = id;
		}
	}
}
