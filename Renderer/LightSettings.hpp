#pragma once

#include "../Core/Math/Vector4.hpp"
#include "../Vendor/entt/entt.hpp"
#include "ComputeShader.hpp"
#include "SSBO.hpp"
#include "../Core/Math/Vector3.hpp"

struct Cluster
{
	Vector4 minBounds;
	Vector4 maxBounds;
};

static const int NUM_LIGHTS = 256;
static const int NUM_TILES = 512;

struct PointLightGLSL
{
	Vector4 position;
	Vector4 color;
	float radius;
	float intensity;
	bool castsShadows;
	float padding[1];
};

struct SpotLightGLSL
{
	Vector4 position;
	Vector4 direction;
	Vector4 color;
	float intensity;
	float cutOff;
	float range;
	float padding[1];
};

struct AmbientLightGLSL
{
	Vector4 color;
	float intensity;
	float padding[3];
};

struct DirectionalLightGLSL
{
	Vector4 direction;
	Vector4 color;
	float intensity;
	float padding[3];
};

struct PointLightData
{
	PointLightGLSL pointLights[NUM_LIGHTS];
	int clusterPointLightCount[NUM_TILES];
	int clusterPointLightIndices[NUM_TILES][NUM_LIGHTS];
};

struct SpotLightData
{
	SpotLightGLSL spotLights[NUM_LIGHTS];
	int clusterSpotLightCount[NUM_TILES];
	int clusterSpotLightIndices[NUM_TILES][NUM_LIGHTS];
};

struct GlobalLightData
{
	AmbientLightGLSL ambientLights[NUM_LIGHTS];
	DirectionalLightGLSL directionalLights[NUM_LIGHTS];
	int numberOfAmbientLights;
	int numberOfDirectionalLights;
	int padding[2];
};

class LightSettings
{
public:
	LightSettings(std::shared_ptr<entt::registry> registry);
	void Update(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const float near, const float far);
private:
	std::shared_ptr<entt::registry> registry;
	ComputeShader* buildLightGrid;
	ComputeShader* cullLights;

	static constexpr uint32_t CLUSTERS_X = 8;
	static constexpr uint32_t CLUSTERS_Y = 8;
	static constexpr uint32_t CLUSTERS_Z = 8;
	static constexpr uint32_t CLUSTER_COUNT = CLUSTERS_X * CLUSTERS_Y * CLUSTERS_Z;

	Cluster clusterData[CLUSTER_COUNT];
	SSBO clustersSSBO;

	PointLightData* pointLightData;
	SSBO pointLightSSBO;

	SpotLightData* spotLightData;
	SSBO spotLightSSBO;

	GlobalLightData* globalLightData;
	SSBO globalLightSSBO;
};
