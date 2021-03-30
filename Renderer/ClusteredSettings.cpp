//#include "ClusteredSettings.hpp"
//#include <glad/glad.h>
//#include "../Core/Transform.hpp"
//
//ClusteredSettings::ClusteredSettings(std::shared_ptr<entt::registry> registry, int numLights)
//{
//	Vector2 minScreenCoord(-1, -1);
//	Vector2 maxScreenCoord(1, 1);
//
//	this->gridSize = Vector3(8, 8, 8);
//	this->camera = camera;
//	this->numLights = numLights;
//	this->minCoord = minScreenCoord;
//	this->registry = registry;
//
//	gridDimensions = Vector3(
//		std::abs(minScreenCoord.x - maxScreenCoord.x) / static_cast<float>(gridSize.x),
//		std::abs(minScreenCoord.y - maxScreenCoord.y) / static_cast<float>(gridSize.y),
//		1.0f / static_cast<float>(gridSize.z));
//
//	numTiles = gridSize.x * gridSize.y * gridSize.z;
//
//	gridCubes = new GridCube[numTiles];
//	//tileBounds = new TileBounds[numTiles];
//
//	compute = new ComputeShader("Resources/Clustered/compute.glsl");
//	dataPrep = new ComputeShader("Resources/Clustered/dataPrep.glsl");
//
//	tileData = new TileData();
//	//directionalLightData = new DirectionalLightData();
//	//ambientLightData = new AmbientLightData();
//	//ambientDirectionalLightData = new AmbientDirectionalLightData();
//	//spotLightData = new SpotLightData();
//	GenerateGrid();
//	InitGridSSBO();
//	InitLightSSBO();
//
//	/*for (int i = 0; i < 512; i++)
//	{
//		std::cout << gridCubes[i].planes[0] << std::endl;
//		std::cout << gridCubes[i].planes[1] << std::endl;
//	}*/
//}
//
//void ClusteredSettings::InitLightSSBO()
//{
//	//
//
//	numLightsInScene = 0;
//
//	registry->view<Transform, PointLight>().each([this](auto& transform, auto& pointLight)
//	{
//		lightData[numLightsInScene].lightPosition = Vector4 (transform.position.x, transform.position.y, transform.position.z, 1);
//		lightData[numLightsInScene].lightColour = pointLight.color;
//		lightData[numLightsInScene].lightRadius = pointLight.radius;
//		lightData[numLightsInScene].intensity = pointLight.intensity;
//		numLightsInScene++;
//	});
//
//	if (!tilelightsSSBO.initialized)
//		tilelightsSSBO.Initialize(2, sizeof(TileData), tileData);
//
//	lightDataSSBO.Set(1, sizeof(LightData) * NUM_LIGHTS, &lightData);
//
//	//
//
//	/*ambientDirectionalLightData->numAmbientLights = 0;
//	ambientDirectionalLightData->numDirectionalLights = 0;
//
//	registry->view<Transform, AmbientLight>().each([this](auto& transform, auto& ambientLight)
//	{
//		ambientDirectionalLightData->ambientLightsColors[ambientDirectionalLightData->numAmbientLights] = Vector4(ambientLight.color.x, ambientLight.color.y, ambientLight.color.z, 0);
//		ambientDirectionalLightData->ambientLightsIntensities[ambientDirectionalLightData->numAmbientLights] = ambientLight.intensity;
//		ambientDirectionalLightData->numAmbientLights++;
//	});
//
//	registry->view<Transform, DirectionalLight>().each([this](auto& transform, auto& directionalLight)
//	{
//		Vector3 forward = transform.GetForward();
//		ambientDirectionalLightData->directionalLightsDirections[ambientDirectionalLightData->numDirectionalLights] = Vector4(forward.x, forward.y, forward.z, 0);
//		ambientDirectionalLightData->directionalLightsColors[ambientDirectionalLightData->numDirectionalLights] = Vector4(directionalLight.color.x, directionalLight.color.y, directionalLight.color.z, 0);
//		ambientDirectionalLightData->directionalLightsIntensities[ambientDirectionalLightData->numDirectionalLights] = directionalLight.intensity;
//		ambientDirectionalLightData->numDirectionalLights++;
//	});
//
//	ambientDirectionalLightDataSSBO.Set(5, sizeof(AmbientDirectionalLightData), &ambientDirectionalLightData);*/
//
//	//DirectionalLightDataSSBO.Set(5, sizeof(DirectionalLightData), directionalLightData);
//
//	//
//
//	numSpotLightsInScene = 0;
//
//	registry->view<Transform, SpotLight>().each([this](auto& transform, auto& spotLight)
//	{
//		Vector3 forward = transform.GetForward();
//		spotLightData[numSpotLightsInScene].lightPosition = Vector4(transform.position.x, transform.position.y, transform.position.z, 1);
//		spotLightData[numSpotLightsInScene].lightDirection = Vector4(forward.x, forward.y, forward.z, 0);
//		spotLightData[numSpotLightsInScene].lightColor = Vector4(spotLight.color.x, spotLight.color.y, spotLight.color.z, 0);
//		spotLightData[numSpotLightsInScene].intensity = spotLight.intensity;
//		spotLightData[numSpotLightsInScene].cutOff = spotLight.cutOff;
//		spotLightData[numSpotLightsInScene].range = spotLight.range;
//		numSpotLightsInScene++;
//	});
//
//	SpotLightDataSSBO.Set(6, sizeof(SpotLightData) * NUM_LIGHTS, &spotLightData);
//
//	/*spotLightData->numSpotLights = 0;
//
//	registry->view<Transform, SpotLight>().each([this](auto& transform, auto& spotLight)
//	{
//		Vector3 forward = transform.GetForward();
//		spotLightData->spotLightPositions[spotLightData->numSpotLights] = Vector4(transform.position.x, transform.position.y, transform.position.z, 0);
//		spotLightData->spotLightDirections[spotLightData->numSpotLights] = Vector4(forward.x, forward.y, forward.z, 0);
//		spotLightData->spotLightColors[spotLightData->numSpotLights] = Vector4(spotLight.color.x, spotLight.color.y, spotLight.color.z, 0);
//		spotLightData->spotLightIntensities[spotLightData->numSpotLights] = spotLight.intensity;
//		spotLightData->spotLightCutOffs[spotLightData->numSpotLights] = spotLight.cutOff;
//		spotLightData->numSpotLights++;
//	});
//
//	SpotLightDataSSBO.Set(6, sizeof(SpotLightData), spotLightData);*/
//
//	//
//
//	/*ambientLightData->numAmbientLights = 0;
//
//	registry->view<Transform, AmbientLight>().each([this](auto& transform, auto& ambientLight)
//	{
//		ambientLightData->ambientLightsColors[ambientLightData->numAmbientLights] = Vector4(ambientLight.color.x, ambientLight.color.y, ambientLight.color.z, 0);
//		ambientLightData->ambientLightsIntensities[ambientLightData->numAmbientLights] = ambientLight.intensity;
//		ambientLightData->numAmbientLights++;
//	});
//	
//	AmbientLightDataSSBO.Set(7, sizeof(AmbientLightData), ambientLightData);*/
//}
//
//void ClusteredSettings::GenerateGrid()
//{
//	float xOffset = 0;
//	float yOffset = 0;
//	numTiles = gridSize.x * gridSize.y * gridSize.z;
//
//	int xIndex = 0;
//	int yIndex = 0;
//	int zIndex = 0;
//
//	for (int i = 0; i < gridSize.x * gridSize.y; i++)
//	{
//		zIndex = 0;
//		xIndex = ceilf(xOffset);
//
//		//Once reached the end of x axis, reset x offset and move up y axis.
//		if (xIndex == gridSize.x)
//		{
//			yOffset += gridDimensions.y;
//			++yIndex;
//			xOffset = 0;
//			xIndex = 0;
//		}
//
//		//Create tile closest to screen.
//		const Vector3 startPosition((gridDimensions.x * xOffset) + minCoord.x, yOffset + minCoord.y, 1.0f);
//		int index = xIndex + int(gridSize.x) * (yIndex + int(gridSize.y) * zIndex);
//
//		float offset = 2.0f / gridSize.x;
//		gridCubes[index].planes[0] = Vector4(-1, 0, 0, Mathf::Lerp(1, -1 + offset, xIndex / (gridSize.x - 1)));
//		gridCubes[index].planes[1] = Vector4(1, 0, 0, Mathf::Lerp(-1 + offset, 1, xIndex / (gridSize.x - 1)));
//		gridCubes[index].planes[2] = Vector4(0,-1, 0, Mathf::Lerp(1, -1 + offset, yIndex / (gridSize.y - 1)));
//		gridCubes[index].planes[3] = Vector4(0, 1, 0, Mathf::Lerp(-1 + offset, 1, yIndex / (gridSize.y - 1)));
//
//		/*gridCubes[index].planes[0] = Vector4(-1, 0, 0, startPosition.Length());
//		gridCubes[index].planes[1] = Vector4(1, 0, 0, (startPosition + Vector3(gridDimensions.x, 0, 0)).Length());
//		gridCubes[index].planes[2] = Vector4(0, 0, -1, (startPosition + Vector3(0, 0, gridDimensions.z)).Length());
//		gridCubes[index].planes[3] = Vector4(0, 0, 1, startPosition.Length());
//		gridCubes[index].planes[4] = Vector4(0, 1, 0, (startPosition + Vector3(0, gridDimensions.y, 0)).Length());
//		gridCubes[index].planes[5] = Vector4(0, -1, 0, startPosition.Length());*/
//
//		//Fill along the z axis from the tile above.
//		for (int k = 1; k <= gridSize.z - 1; ++k)
//		{
//			zIndex = k;
//			const float newZCoord = (gridDimensions.z * k);
//			index = xIndex + int(gridSize.x) * (yIndex + int(gridSize.y) * zIndex);
//			const Vector3 positionExtendedInZAxis(startPosition.x, startPosition.y, startPosition.z + newZCoord);
//
//			//std::cout << xIndex << " | " << yIndex << " | " << zIndex << std::endl;
//			//std::cout << xIndex << " | " << XnegativeW << " | " << XpositiveW << std::endl;
//			float offset = 2.0f / gridSize.x;
//			gridCubes[index].planes[0] = Vector4(-1, 0, 0, Mathf::Lerp(1, -1 + offset, xIndex / (gridSize.x - 1)));
//			gridCubes[index].planes[1] = Vector4(1, 0, 0, Mathf::Lerp(-1 + offset, 1, xIndex / (gridSize.x - 1)));
//			gridCubes[index].planes[2] = Vector4(0, -1, 0, Mathf::Lerp(1, -1 + offset, yIndex / (gridSize.y - 1)));
//			gridCubes[index].planes[3] = Vector4(0, 1, 0, Mathf::Lerp(-1 + offset, 1, yIndex / (gridSize.y - 1)));
//
//			/*gridCubes[index].planes[0] = Vector4(-1, 0, 0, positionExtendedInZAxis.Length());
//			gridCubes[index].planes[1] = Vector4(1, 0, 0, (positionExtendedInZAxis + Vector3(gridDimensions.x, 0, 0)).Length());
//			gridCubes[index].planes[2] = Vector4(0, 0, -1, (positionExtendedInZAxis + Vector3(0, 0, gridDimensions.z)).Length());
//			gridCubes[index].planes[3] = Vector4(0, 0, 1, positionExtendedInZAxis.Length());
//			gridCubes[index].planes[4] = Vector4(0, 1, 0, (positionExtendedInZAxis + Vector3(0, gridDimensions.y, 0)).Length());
//			gridCubes[index].planes[5] = Vector4(0, -1, 0, positionExtendedInZAxis.Length());*/
//		}
//
//		++xOffset;
//	}
//}
//
//void ClusteredSettings::InitGridSSBO()
//{
//	gridCubesSSBO.Initialize(4, sizeof(GridCube) * numTiles, gridCubes);
//	//tileBoundsSSBO.Initialize(4, sizeof(TileBounds) * numTiles, tileBounds);
//	screenSpaceDataSSBO.Initialize(3, sizeof(ScreenSpaceData), &ssdata);
//
//	countBuffer.Initialize(0,2);
//}
//
//void ClusteredSettings::Update(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far, const float aspectRatio)
//{
//	countBuffer.SetAll(0);
//
//	InitLightSSBO();
//	PrepareDataGPU(projectionMatrix, viewMatrix, cameraPos, near, far, aspectRatio);
//	FillTilesGPU(projectionMatrix, viewMatrix, cameraPos, near, far);
//
//	/*registry->view<Transform, PointLight>().each([this, projectionMatrix, viewMatrix](auto& transform, auto& pointLight)
//	{
//		std::cout << (viewMatrix * Vector4(transform.position.x, transform.position.y, transform.position.z, 1)).Length() << std::endl;
//
//		std::cout << "end" << std::endl;
//	});*/
//}
//
//void ClusteredSettings::FillTilesGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far) const
//{
//	compute->Use();
//
//	compute->SetInt("numZTiles", gridSize.z);
//	compute->SetFloat("nearPlane", near);
//	compute->SetFloat("farPlane", far);
//	compute->SetMatrix4x4("projMatrix", projectionMatrix);
//	compute->SetMatrix4x4("viewMatrix", viewMatrix);
//	compute->SetVector4("cameraPosition", cameraPos.x, cameraPos.y, cameraPos.z, 0);
//
//	compute->Dispatch(8,8,8);
//}
//
//void ClusteredSettings::PrepareDataGPU(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const Vector3& cameraPos, const float near, const float far, const float aspectRatio) const
//{
//	dataPrep->Use();
//
//	dataPrep->SetInt("lightsInScene", numLightsInScene);
//	dataPrep->SetInt("spotLightsInScene", numSpotLightsInScene);
//	dataPrep->SetFloat("nearPlane", near);
//	dataPrep->SetFloat("farPlane", far);
//	dataPrep->SetMatrix4x4("projectionMatrix", projectionMatrix);
//	dataPrep->SetMatrix4x4("viewMatrix", viewMatrix);
//	dataPrep->SetMatrix4x4("projView", projectionMatrix * viewMatrix);
//	dataPrep->SetFloat("aspectRatio", aspectRatio);
//
//	dataPrep->Dispatch(8,8,8);
//}
