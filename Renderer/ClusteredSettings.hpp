#pragma once

#include "../Core/Math/Vector2.hpp"
#include "../Core/Math/Vector3.hpp"
#include "../Core/Math/Vector4.hpp"
#include "../Core/Math/Matrix4x4.hpp"

#include "AmbientLight.hpp"
#include "PointLight.hpp"
#include "DirectionalLight.hpp"
#include "SpotLight.hpp"
#include "ComputeShader.hpp"
#include "Camera.hpp"
#include "../Vendor/entt/entt.hpp"
#include "SSBO.hpp"

#include <glad/glad.h>

static const int NUM_TILES = 512;
static const int NUM_LIGHTS = 512;

struct TileData
{
	int lightIndexes[NUM_TILES];
	int tileLights[NUM_TILES][NUM_LIGHTS];
};

struct ScreenSpaceData
{
	float indexes[NUM_LIGHTS];
	float zRadius[NUM_LIGHTS];
	Vector4 numLightsIn;
	Vector4 NDCCoords[NUM_LIGHTS];
};

struct LightData
{
	Vector4 lightPosition;
	Vector4 lightColour;
	float lightRadius;
	float intensity;

	float padding[2];
};

struct AmbientLightData
{
	Vector4 ambientLightsColors[NUM_LIGHTS];
	float ambientLightsIntensities[NUM_LIGHTS];
	int numAmbientLights;
	int padding[3];
};

struct DirectionalLightData
{
	Vector4 directionalLightsDirections[NUM_LIGHTS];
	Vector4 directionalLightsColors[NUM_LIGHTS];
	float directionalLightsIntensities[NUM_LIGHTS];
	int numDirectionalLights;
	int padding[3];
};

struct SpotLightData
{
	Vector4 spotLightPositions[NUM_LIGHTS];
	Vector4 spotLightDirections[NUM_LIGHTS];
	Vector4 spotLightColors[NUM_LIGHTS];
	float spotLightIntensities[NUM_LIGHTS];
	float spotLightCutOffs[NUM_LIGHTS];
	int numSpotLights;
	int _spotLightDataBufferPadding[3];
};

struct TileBounds
{
	float left, right, top, bottom, front, back;
	float padding[2];
};

class ClusteredSettings
{
public:
	ClusteredSettings(std::shared_ptr<entt::registry> registry, int numLights);
	~ClusteredSettings()
	{
		delete compute;
	}

	TileData* GetTileData() const
	{
		return tileData;
	}

	int GetNumTiles()  const
	{
		return numTiles;
	}

	void GenerateGrid();
	void InitGridSSBO();

	void Update(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far);

	ComputeShader* dataPrep;
	ComputeShader* compute;
	Vector3** dataPrepWorkGroups;
private:
	void PrepareDataGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far) const;
	void FillTilesGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far) const;
	void InitLightSSBO();

	std::shared_ptr<entt::registry> registry;
	LightData lightData[NUM_LIGHTS];
	Camera* camera;
	Vector2 minCoord;

	int numLights;
	int numTiles;
	int numLightsInFrustum = 0;
	int numLightsInScene;

	Vector3 gridSize;
	Vector3 gridDimensions;

	//Data
	TileBounds* tileBounds;

	//Cube screenCube;
	TileData* tileData;
	DirectionalLightData* directionalLightData;
	SpotLightData* spotLightData;
	AmbientLightData* ambientLightData;

	Vector4 screenLightData[NUM_LIGHTS];
	ScreenSpaceData ssdata;

	//SSBO Stuff
	//GLuint tileBoundsSSBO;
	//GLuint screenSpaceDataSSBO;
	//GLuint tilelightssssbo;

	SSBO screenSpaceDataSSBO;
	SSBO tilelightsSSBO;
	SSBO tileBoundsSSBO;
	SSBO lightDataSSBO;
	SSBO DirectionalLightDataSSBO;
	SSBO SpotLightDataSSBO;
	SSBO AmbientLightDataSSBO;

	GLuint countBuffer;
	GLuint count;

	//Uniform location
	//GLuint loc_numZTiles;
	GLuint loc_projMatrix;
	GLuint loc_projView;
	GLuint loc_cameraPos;

	int inds[100];
};