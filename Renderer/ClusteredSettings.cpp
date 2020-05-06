#include "ClusteredSettings.hpp"
#include <glad/glad.h>

#include "../Core/Transform.hpp"

ClusteredSettings::ClusteredSettings(std::shared_ptr<entt::registry> registry, int numLights)
{
	Vector2 minScreenCoord(-1, -1);
	Vector2 maxScreenCoord(1, 1);

	this->gridSize = Vector3(8, 8, 8);
	this->camera = camera;
	this->numLights = numLights;
	this->minCoord = minScreenCoord;
	this->registry = registry;

	//lightData = new LightData[NUM_LIGHTS];

	gridDimensions = Vector3(
		std::abs(minScreenCoord.x - maxScreenCoord.x) / static_cast<float>(gridSize.x),
		std::abs(minScreenCoord.y - maxScreenCoord.y) / static_cast<float>(gridSize.y),
		1.0f / static_cast<float>(gridSize.z));

	numTiles = gridSize.x * gridSize.y * gridSize.z;

	//gridPlanes = new CubePlanes[numTiles];
	tileBounds = new TileBounds[numTiles];

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
	numLightsInScene = 0;

	registry->view<Transform, PointLight>().each([this](auto& transform, auto& pointLight)
	{
		lightData[numLightsInScene].lightPosition = Vector4 (transform.position.x, transform.position.y, transform.position.z, 1);
		lightData[numLightsInScene].lightColour = pointLight.color;
		lightData[numLightsInScene].lightRadius = pointLight.radius;
		lightData[numLightsInScene].intensity = pointLight.intensity;
		numLightsInScene++;
	});

	tilelightsSSBO.Initialize(3, sizeof(TileData), tileData);
	lightDataSSBO.Initialize(1, sizeof(LightData) * NUM_LIGHTS, &lightData);
}

void ClusteredSettings::GenerateGrid()
{
	float xOffset = 0;
	float yOffset = 0;
	numTiles = gridSize.x * gridSize.y * gridSize.z;

	int xIndex = 0;
	int yIndex = 0;
	int zIndex = 0;

	for (int i = 0; i < gridSize.x * gridSize.y; i++)
	{
		zIndex = 0;
		xIndex = ceilf(xOffset);

		//Once reached the end of x axis, reset x offset and move up y axis.
		if (xIndex == gridSize.x)
		{
			yOffset += gridDimensions.y;
			++yIndex;
			xOffset = 0;
			xIndex = 0;
		}

		//Create tile closest to screen.
		const Vector3 startPosition((gridDimensions.x * xOffset) + minCoord.x, yOffset + minCoord.y, 1.0f);
		int index = xIndex + int(gridSize.x) * (yIndex + int(gridSize.y) * zIndex);

		tileBounds[index].left = startPosition.x;
		tileBounds[index].right = startPosition.x + gridDimensions.x;
		tileBounds[index].top = startPosition.y;
		tileBounds[index].bottom = startPosition.y + gridDimensions.y;
		tileBounds[index].front = startPosition.z;
		tileBounds[index].back = startPosition.z + gridDimensions.z;

		//Fill along the z axis from the tile above.
		for (int k = 1; k <= gridSize.z - 1; ++k)
		{
			zIndex = k;
			const float newZCoord = startPosition.z + (gridDimensions.z * k);
			index = xIndex + int(gridSize.x) * (yIndex + int(gridSize.y) * zIndex);

			tileBounds[index].left = startPosition.x;
			tileBounds[index].right = startPosition.x + gridDimensions.x;
			tileBounds[index].top = startPosition.y;
			tileBounds[index].bottom = startPosition.y + gridDimensions.y;
			tileBounds[index].front = newZCoord;
			tileBounds[index].back = newZCoord + gridDimensions.z;
		}

		++xOffset;
	}
}

void ClusteredSettings::InitGridSSBO()
{
	tileBoundsSSBO.Initialize(6, sizeof(TileBounds) * numTiles, tileBounds);
	screenSpaceDataSSBO.Initialize(5, sizeof(ScreenSpaceData), &ssdata);

	glGenBuffers(1, &countBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, countBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, countBuffer);
}

void ClusteredSettings::Update(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far) const
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, countBuffer);
	glInvalidateBufferData(countBuffer);
	GLuint zero = 0;
	glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	PrepareDataGPU(projectionMatrix, viewMatrix, cameraPos, near, far);
	FillTilesGPU(projectionMatrix, viewMatrix, cameraPos, near, far);
}

void ClusteredSettings::FillTilesGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far) const
{
	//Writes to the shared buffer used in lighting pass
	compute->Use();

	glUniform1i(loc_numZTiles, gridSize.z);
	glUniform1i(glGetUniformLocation(compute->GetProgramID(), "forceGlobalLight"), 0);
	glUniform1f(glGetUniformLocation(compute->GetProgramID(), "nearPlane"), near);
	glUniform1f(glGetUniformLocation(compute->GetProgramID(), "farPlane"), far);
	glUniformMatrix4fv(glGetUniformLocation(compute->GetProgramID(), "projMatrix"), 1, false, (float*)& projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(compute->GetProgramID(), "viewMatrix"), 1, false, (float*)& viewMatrix);

	float vec4[4] = { cameraPos.x, cameraPos.y, cameraPos.z, 0 };
	glUniform4fv(glGetUniformLocation(compute->GetProgramID(), "cameraPosition"), 1, vec4);

	compute->Dispatch(8,8,8);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
}

void ClusteredSettings::PrepareDataGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far) const
{
	Matrix4x4 projView = projectionMatrix * viewMatrix;
	dataPrep->Use();

	glUniform1i(glGetUniformLocation(dataPrep->GetProgramID(), "lightsInScene"), numLightsInScene);

	glUniform1f(glGetUniformLocation(dataPrep->GetProgramID(), "nearPlane"), near);
	glUniform1f(glGetUniformLocation(dataPrep->GetProgramID(), "farPlane"), far);
	glUniformMatrix4fv(loc_projMatrix, 1, false, (float*)& projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(dataPrep->GetProgramID(), "viewMatrix"), 1, false, (float*)& viewMatrix);
	glUniformMatrix4fv(loc_projView, 1, false, (float*)& projView);

	dataPrep->Dispatch(8,8,8);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
}