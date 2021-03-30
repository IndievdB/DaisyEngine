//#pragma once
//
//#include "../Core/Math/Vector2.hpp"
//#include "../Core/Math/Vector3.hpp"
//#include "../Core/Math/Vector4.hpp"
//#include "../Core/Math/Matrix4x4.hpp"
//
//#include "AmbientLight.hpp"
//#include "PointLight.hpp"
//#include "DirectionalLight.hpp"
//#include "SpotLight.hpp"
//#include "ComputeShader.hpp"
//#include "Camera.hpp"
//#include "../Vendor/entt/entt.hpp"
//#include "SSBO.hpp"
//#include "AtomicCounterBuffer.hpp"
//
//#include <glad/glad.h>
//
//static const int NUM_TILES = 512;
//static const int NUM_LIGHTS = 512;
//
//struct TileData
//{
//	int lightIndexes[NUM_TILES]; // number of lights in tile i
//	int tileLights[NUM_TILES][NUM_LIGHTS]; // the indcies of the lights in tile i
//
//	int spotLightIndexes[NUM_TILES]; // number of lights in tile i
//	int tileSpotLights[NUM_TILES][NUM_LIGHTS]; // the indcies of the lights in tile i
//};
//
//struct ScreenSpaceData
//{
//	float indexes[NUM_LIGHTS];
//	float zRadius[NUM_LIGHTS];
//	Vector4 NDCCoords[NUM_LIGHTS];
//	float indexesSL[NUM_LIGHTS];
//};
//
//struct LightData
//{
//	Vector4 lightPosition;
//	Vector4 lightColour;
//	float lightRadius;
//	float intensity;
//	float padding[2];
//};
//
///*struct AmbientDirectionalLightData
//{
//	Vector4 ambientLightsColors[NUM_LIGHTS];
//	float ambientLightsIntensities[NUM_LIGHTS];
//
//	Vector4 directionalLightsDirections[NUM_LIGHTS];
//	Vector4 directionalLightsColors[NUM_LIGHTS];
//	float directionalLightsIntensities[NUM_LIGHTS];
//
//	int numAmbientLights;
//	int numDirectionalLights;
//	int padding[2];
//};*/
//
///*struct DirectionalLightData
//{
//	int padding[3];
//};*/
//
//struct SpotLightData
//{
//	Vector4 lightPosition;
//	Vector4 lightDirection;
//	Vector4 lightColor;
//	float intensity;
//	float cutOff;
//	float range;
//	float padding[1];
//};
//
///*struct SpotLightData
//{
//	Vector4 spotLightPositions[NUM_LIGHTS];
//	Vector4 spotLightDirections[NUM_LIGHTS];
//	Vector4 spotLightColors[NUM_LIGHTS];
//	float spotLightIntensities[NUM_LIGHTS];
//	float spotLightCutOffs[NUM_LIGHTS];
//	int numSpotLights;
//	int _spotLightDataBufferPadding[3];
//};*/
//
///*struct TileBounds
//{
//	float left, right, top, bottom, front, back;
//	float padding[2];
//};*/
//
//struct GridCube
//{
//	Vector4 planes[6];
//};
//
//class ClusteredSettings
//{
//public:
//	ClusteredSettings(std::shared_ptr<entt::registry> registry, int numLights);
//	~ClusteredSettings()
//	{
//		delete compute;
//	}
//
//	TileData* GetTileData() const
//	{
//		return tileData;
//	}
//
//	int GetNumTiles()  const
//	{
//		return numTiles;
//	}
//
//	void GenerateGrid();
//	void InitGridSSBO();
//
//	void Update(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far, const float aspectRatio);
//
//	ComputeShader* dataPrep;
//	ComputeShader* compute;
//	Vector3** dataPrepWorkGroups;
//private:
//	void PrepareDataGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far, const float aspectRatio) const;
//	void FillTilesGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far) const;
//	void InitLightSSBO();
//
//	std::shared_ptr<entt::registry> registry;
//	LightData lightData[NUM_LIGHTS];
//	SpotLightData spotLightData[NUM_LIGHTS];
//	Camera* camera;
//	Vector2 minCoord;
//
//	int numLights;
//	int numTiles;
//	int numLightsInFrustum = 0;
//	int numLightsInScene;
//	int numSpotLightsInScene;
//
//	Vector3 gridSize;
//	Vector3 gridDimensions;
//
//	//Data
//	//TileBounds* tileBounds;
//	GridCube* gridCubes;
//
//	//Cube screenCube;
//	TileData* tileData;
//	//AmbientDirectionalLightData* ambientDirectionalLightData;
//	//DirectionalLightData* directionalLightData;
//	//SpotLightData* spotLightData;
//	//AmbientLightData* ambientLightData;
//
//	Vector4 screenLightData[NUM_LIGHTS];
//	ScreenSpaceData ssdata;
//
//	//SSBO Stuff
//	SSBO screenSpaceDataSSBO;
//	SSBO tilelightsSSBO;
//	//SSBO tileBoundsSSBO;
//	SSBO gridCubesSSBO;
//	SSBO lightDataSSBO;
//	SSBO SpotLightDataSSBO;
//
//	SSBO ambientDirectionalLightDataSSBO;
//	//SSBO DirectionalLightDataSSBO;
//	//SSBO AmbientLightDataSSBO;
//
//	AtomicCounterBuffer countBuffer;
//
//	//Uniform location
//	//GLuint loc_numZTiles;
//	GLuint loc_projMatrix;
//	GLuint loc_projView;
//	GLuint loc_cameraPos;
//
//	int inds[100];
//};