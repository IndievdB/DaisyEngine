#pragma once

#include "../Core/Math/Vector2.hpp"
#include "../Core/Math/Vector4.hpp"
#include "../Core/Math/Matrix4x4.hpp"
#include "../Temp/GridUtility.h"

#include "PointLight.hpp"
#include "ComputeShader.hpp"
#include "Camera.hpp"
#include "../Vendor/entt/entt.hpp"

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

class ClusteredSettings
{
public:
	ClusteredSettings(std::shared_ptr<entt::registry> registry, int numLights, int numXTiles, int numYTiles, int numZTiles, Vector2 minScreenCoord, Vector2 maxScreenCoord);
	~ClusteredSettings()
	{
		delete compute;
	}


	TileData* GetTileData() const
	{
		return tileData;
	}

	Tile* GetScreenTiles()
	{
		return screenTiles;
	}

	int GetNumTiles()  const
	{
		return numTiles;
	}

	void GenerateGrid();
	void InitGridSSBO();

	void Update(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far) const;

	ComputeShader* dataPrep;
	ComputeShader* compute;
	Vector3** dataPrepWorkGroups;
private:
	void PrepareDataGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far) const;
	void FillTilesGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far) const;
	void InitLightSSBO();

	std::shared_ptr<entt::registry> registry;
	LightData lightData[NUM_LIGHTS];
	//PointLight** lights;
	Camera* camera;
	Vector2 minCoord;

	int numLights;
	int numTiles;
	int numLightsInFrustum = 0;
	int numLightsInScene;

	Vector3 gridSize;
	Vector3 gridDimensions;

	//Data
	Tile screenTiles[NUM_TILES];
	Cube grid[NUM_TILES];
	CubePlanes* gridPlanes;

	Cube screenCube;
	TileData* tileData;

	Vector4 screenLightData[NUM_LIGHTS];
	ScreenSpaceData ssdata;

	//SSBO Stuff
	GLuint tileDataSSBO;
	GLuint gridPlanesSSBO;
	GLuint screenSpaceDataSSBO;
	GLuint modelMatricesSSBO;
	
	GLuint ssbo;
	GLuint tilelightssssbo;

	GLuint countBuffer;
	GLuint count;

	//Uniform location
	GLuint loc_numZTiles;
	GLuint loc_projMatrix;
	GLuint loc_projView;
	GLuint loc_cameraPos;

	int inds[100];
};