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

	gridDimensions = Vector3(
		std::abs(minScreenCoord.x - maxScreenCoord.x) / static_cast<float>(gridSize.x),
		std::abs(minScreenCoord.y - maxScreenCoord.y) / static_cast<float>(gridSize.y),
		1.0f / static_cast<float>(gridSize.z));

	numTiles = gridSize.x * gridSize.y * gridSize.z;

	tileBounds = new TileBounds[numTiles];

	compute = new ComputeShader("Resources/Clustered/compute.glsl");
	dataPrep = new ComputeShader("Resources/Clustered/dataPrep.glsl");

	tileData = new TileData();
	directionalLightData = new DirectionalLightData();
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

	if (!tilelightsSSBO.initialized)
		tilelightsSSBO.Initialize(2, sizeof(TileData), tileData);

	lightDataSSBO.Set(1, sizeof(LightData) * NUM_LIGHTS, &lightData);

	directionalLightData->numDirectionalLights = 0;

	registry->view<Transform, DirectionalLight>().each([this](auto& transform, auto& directionalLight)
	{
		Vector3 forward = transform.GetForward();
		directionalLightData->directionalLightsDirections[directionalLightData->numDirectionalLights] = Vector4(forward.x, forward.y, forward.z, 0);
		directionalLightData->directionalLightsColors[directionalLightData->numDirectionalLights] = Vector4(directionalLight.color.x, directionalLight.color.y, directionalLight.color.z, 0);
		directionalLightData->directionalLightsIntensities[directionalLightData->numDirectionalLights] = directionalLight.intensity;
		directionalLightData->numDirectionalLights++;
	});

	DirectionalLightDataSSBO.Set(5, sizeof(DirectionalLightData), directionalLightData);
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
	tileBoundsSSBO.Initialize(4, sizeof(TileBounds) * numTiles, tileBounds);
	screenSpaceDataSSBO.Initialize(3, sizeof(ScreenSpaceData), &ssdata);

	glGenBuffers(1, &countBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, countBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, countBuffer);
}

void ClusteredSettings::Update(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far)
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, countBuffer);
	glInvalidateBufferData(countBuffer);
	GLuint zero = 0;
	glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	InitLightSSBO();
	PrepareDataGPU(projectionMatrix, viewMatrix, cameraPos, near, far);
	FillTilesGPU(projectionMatrix, viewMatrix, cameraPos, near, far);
}

void ClusteredSettings::FillTilesGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far) const
{
	compute->Use();

	compute->SetInt("numZTiles", gridSize.z);
	compute->SetFloat("nearPlane", near);
	compute->SetFloat("farPlane", far);
	compute->SetMatrix4x4("projMatrix", projectionMatrix);
	compute->SetMatrix4x4("viewMatrix", viewMatrix);
	compute->SetVector4("cameraPosition", cameraPos.x, cameraPos.y, cameraPos.z, 0);

	compute->Dispatch(8,8,8);
}

void ClusteredSettings::PrepareDataGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far) const
{
	dataPrep->Use();

	dataPrep->SetInt("lightsInScene", numLightsInScene);
	dataPrep->SetFloat("nearPlane", near);
	dataPrep->SetFloat("farPlane", far);
	dataPrep->SetMatrix4x4("projectionMatrix", projectionMatrix);
	dataPrep->SetMatrix4x4("viewMatrix", viewMatrix);
	dataPrep->SetMatrix4x4("projView", projectionMatrix * viewMatrix);

	dataPrep->Dispatch(8,8,8);
}