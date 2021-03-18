#version 430 core

#include Resources/Clustered/configuration.glsl

uniform float nearPlane;
uniform float farPlane;
uniform mat4 projectionMatrix;
uniform mat4 projView;
uniform mat4 viewMatrix;
uniform int lightsInScene;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct LightData
{
	vec4 pos4;
	vec4 lightColour;
	float lightRadius;
	float intensity;

	float fpadding[2];
};

layout(binding = 0) uniform atomic_uint count;

//Shared with lighting shader
layout(std430, binding = 1) buffer LightDataBuffer
{
	LightData lightData[];
};

layout(std430, binding = 3) buffer ScreenSpaceDataBuffer
{
	float indexes[numLights];
	float zRadius[numLights];
	vec4 numLightsIn;
	vec4 NDCCoords[];
};

#include Resources/Clustered/collisionFunctions.glsl

void main()
{
	int xIndex = int(gl_GlobalInvocationID.x);
	int yIndex = int(gl_GlobalInvocationID.y);
	int zIndex = int(gl_GlobalInvocationID.z);

	int id = xIndex + int(gl_NumWorkGroups.x) * (yIndex + int(gl_NumWorkGroups.y) * zIndex);

	if (id >= lightsInScene)
		return;

	vec4 worldLight = vec4(lightData[id].pos4.xyz, 1.0f);

	vec4 frustum[6];
	FrustumFromMatrix(projView, frustum);
	bool colliding = SphereCubeColliding(frustum, vec4(worldLight.xyz, lightData[id].lightRadius));

	//If light affects any clusters on screen, send to next shader for allocation, 
	//else cull.
	if (colliding)
	{
		vec4 projViewPos = projView * worldLight;
		vec4 viewPos = viewMatrix * worldLight;
		float zCoord = -(viewPos.z - nearPlane) / farPlane + nearPlane;

		//Store reciprocal to avoid use of division below.
		float w = 1.0f / projViewPos.w;

		//Final screenspace data.
		//float radius = lightData[id].lightRadius * w * 2.0f;
		float fovy = 2 * atan(1/projectionMatrix[1].y);
		float radius = (lightData[id].lightRadius / (tan((fovy/2.0f)) * abs(viewPos.z)));

		vec4 S = viewMatrix * (worldLight + vec4(lightData[id].lightRadius,0,0,0));
		float viewRadius = distance (viewPos.xyz, S.xyz);
		vec4 S1 = projectionMatrix * (viewPos + vec4(viewRadius,0,0,0));

		//vec4 projViewPosPlusRadius = projectionMatrix * viewPos;
		//float radius = distance(projViewPos, S1);

		vec4 clipPos = vec4(projViewPos.x * w, projViewPos.y * w, zCoord, radius);

		uint currentLightCount = atomicCounterIncrement(count);

		NDCCoords[currentLightCount] = clipPos;
		zRadius[currentLightCount] = lightData[id].lightRadius / (farPlane - nearPlane);
		indexes[currentLightCount] = id;
	}
}
