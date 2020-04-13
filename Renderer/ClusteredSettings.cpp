#include "ClusteredSettings.hpp"
#include <glad/glad.h>

#include "../Core/Transform.hpp"
#include "../Temp/GLUtil.h"

ClusteredSettings::ClusteredSettings(std::shared_ptr<entt::registry> registry, int numLights, int numXTiles, int numYTiles, int numZTiles, Vector2 minScreenCoord, Vector2 maxScreenCoord)
{
	this->camera = camera;
	this->numLights = numLights;
	this->minCoord = minScreenCoord;
	this->registry = registry;

	gridSize = Vector3(numXTiles, numYTiles, numZTiles);
	gridDimensions = Vector3(
		std::abs(minScreenCoord.x - maxScreenCoord.x) / static_cast<float>(gridSize.x),
		std::abs(minScreenCoord.y - maxScreenCoord.y) / static_cast<float>(gridSize.y),
		1.0f / static_cast<float>(gridSize.z));

	numTiles = gridSize.x * gridSize.y * gridSize.z;

	gridPlanes = new CubePlanes[numTiles];

	compute = new ComputeShader("Resources/Clustered/compute.glsl");
	loc_numZTiles = glGetUniformLocation(compute->GetProgramID(), "numZTiles");

	dataPrep = new ComputeShader("Resources/Clustered/dataPrep.glsl");
	loc_projMatrix = glGetUniformLocation(dataPrep->GetProgramID(), "projectionMatrix");
	loc_projView = glGetUniformLocation(dataPrep->GetProgramID(), "projView");
	loc_cameraPos = glGetUniformLocation(dataPrep->GetProgramID(), "cameraPos");

	tileData = new TileData();
	GenerateGrid();
	InitGridSSBO();

	InitLightSSBO();
}

void ClusteredSettings::InitLightSSBO()
{
	int lightIndex = 0;

	registry->view<Transform, PointLight>().each([this, &lightIndex](auto& transform, auto& pointLight)
	{
		lightData[lightIndex].lightPosition = Vector4 (transform.position.x, transform.position.y, transform.position.z, 1);
		lightData[lightIndex].lightColour = pointLight.color;
		lightData[lightIndex].lightRadius = pointLight.radius;
		lightData[lightIndex].intensity = pointLight.intensity;
		lightIndex++;
	});

	ssbo = GLUtil::InitSSBO(1, 1, ssbo, sizeof(LightData) * NUM_LIGHTS, &lightData, GL_STATIC_COPY);
	tilelightssssbo = GLUtil::InitSSBO(1, 3, tilelightssssbo, sizeof(TileData), tileData, GL_STATIC_COPY);

	//TEMP?
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightData) * NUM_LIGHTS, &lightData, GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ClusteredSettings::GenerateGrid()
{
	const GridData gridData(grid, gridPlanes, screenTiles, minCoord);
	GridUtility::Generate3DGrid(gridData, gridDimensions, gridSize);
}

void ClusteredSettings::InitGridSSBO()
{
	gridPlanesSSBO = GLUtil::InitSSBO(1, 4, gridPlanesSSBO,
		sizeof(CubePlanes) * numTiles, gridPlanes, GL_STATIC_COPY);

	screenSpaceDataSSBO = GLUtil::InitSSBO(1, 5, screenSpaceDataSSBO,
		sizeof(ScreenSpaceData), &ssdata, GL_STATIC_COPY);

	glGenBuffers(1, &countBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, countBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, countBuffer);
}

void ClusteredSettings::Update(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos) const
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, countBuffer);
	glInvalidateBufferData(countBuffer);
	GLuint zero = 0;
	glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	PrepareDataGPU(projectionMatrix, viewMatrix, cameraPos);
	FillTilesGPU(projectionMatrix, viewMatrix, cameraPos);
}

void ClusteredSettings::FillTilesGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos) const
{
	//Writes to the shared buffer used in lighting pass
	compute->Use();

	glUniform1i(loc_numZTiles, gridSize.z);
	glUniform1i(glGetUniformLocation(compute->GetProgramID(), "forceGlobalLight"), 0);
	glUniform1f(glGetUniformLocation(compute->GetProgramID(), "nearPlane"), 1.0f);
	glUniform1f(glGetUniformLocation(compute->GetProgramID(), "farPlane"), 4000.0f);
	glUniformMatrix4fv(glGetUniformLocation(compute->GetProgramID(), "projMatrix"), 1, false, (float*)& projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(compute->GetProgramID(), "viewMatrix"), 1, false, (float*)& viewMatrix);

	float vec4[4] = { cameraPos.x, cameraPos.y, cameraPos.z, 0 };
	glUniform4fv(glGetUniformLocation(compute->GetProgramID(), "cameraPosition"), 1, vec4);

	compute->Dispatch(8,8,8);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
}

void ClusteredSettings::PrepareDataGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos) const
{
	Matrix4x4 projView = projectionMatrix * viewMatrix;
	dataPrep->Use();

	glUniform1f(glGetUniformLocation(dataPrep->GetProgramID(), "nearPlane"), 1.0f);
	glUniform1f(glGetUniformLocation(dataPrep->GetProgramID(), "farPlane"), 4000.0f);
	glUniformMatrix4fv(loc_projMatrix, 1, false, (float*)& projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(dataPrep->GetProgramID(), "viewMatrix"), 1, false, (float*)& viewMatrix);
	glUniformMatrix4fv(loc_projView, 1, false, (float*)& projView);

	dataPrep->Dispatch(8,8,8);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
}