#include "LightSettings.hpp"
#include "Window.hpp"

#include "PointLight.hpp"
#include "SpotLight.hpp"
#include "AmbientLight.hpp"
#include "DirectionalLight.hpp"
#include "../Core/Transform.hpp"

LightSettings::LightSettings(std::shared_ptr<entt::registry> registry)
{
	this->registry = registry;
	buildLightGrid = new ComputeShader("Resources/Engine/Compute/BuildLightGrid.glsl");
	cullLights = new ComputeShader("Resources/Engine/Compute/CullLights.glsl");

	clustersSSBO.Initialize(0, sizeof(Cluster) * CLUSTER_COUNT, &clusterData);
	pointLightData = new PointLightData();
	spotLightData = new SpotLightData();
	globalLightData = new GlobalLightData();
}

bool once = false;

void LightSettings::Update(const Matrix4x4& projectionMatrix, const Matrix4x4& viewMatrix, const float near, const float far)
{
	// Build Grid

	buildLightGrid->Use();

	buildLightGrid->SetVector2("clusterScreenSpaceSize",
		std::ceil((float)Window::GetInstance()->GetViewportWidth() / (float)CLUSTERS_X),
		std::ceil((float)Window::GetInstance()->GetViewportHeight() / (float)CLUSTERS_Y));
	buildLightGrid->SetMatrix4x4("inverseProjection", Matrix4x4::AffineInverse(projectionMatrix));
	buildLightGrid->SetFloat("nearPlane", near);
	buildLightGrid->SetFloat("farPlane", far);
	buildLightGrid->SetFloat("screenWidth", Window::GetInstance()->GetViewportWidth());
	buildLightGrid->SetFloat("screenHeight", Window::GetInstance()->GetViewportHeight());
	buildLightGrid->SetUVector3("clusterSizes", CLUSTERS_X, CLUSTERS_Y, CLUSTERS_Z);

	buildLightGrid->Dispatch(CLUSTERS_X, CLUSTERS_Y, CLUSTERS_Z);

	// Fill Point Light Buffer

	int numPointLightsInScene = 0;

	registry->view<Transform, PointLight>().each([this, &numPointLightsInScene](auto& transform, auto& pointLight)
	{
		pointLightData->pointLights[numPointLightsInScene].position = Vector4 (transform.position.x, transform.position.y, transform.position.z, 1);
		pointLightData->pointLights[numPointLightsInScene].color = pointLight.color;
		pointLightData->pointLights[numPointLightsInScene].radius = pointLight.radius;
		pointLightData->pointLights[numPointLightsInScene].intensity = pointLight.intensity;
		numPointLightsInScene++;
	});

	pointLightSSBO.Set(1, sizeof(PointLightData), pointLightData);

	// Fill Spot Light Buffer

	int numSpotLightsInScene = 0;

	registry->view<Transform, SpotLight>().each([this, &numSpotLightsInScene](auto& transform, auto& spotLight)
	{
		Vector3 forward = transform.GetForward();
		spotLightData->spotLights[numSpotLightsInScene].position = Vector4(transform.position.x, transform.position.y, transform.position.z, 1);
		spotLightData->spotLights[numSpotLightsInScene].direction = Vector4(forward.x, forward.y, forward.z, 0);
		spotLightData->spotLights[numSpotLightsInScene].color = Vector4(spotLight.color.x, spotLight.color.y, spotLight.color.z, 0);
		spotLightData->spotLights[numSpotLightsInScene].intensity = spotLight.intensity;
		spotLightData->spotLights[numSpotLightsInScene].cutOff = spotLight.cutOff;
		spotLightData->spotLights[numSpotLightsInScene].range = spotLight.range;
		numSpotLightsInScene++;
	});

	spotLightSSBO.Set(2, sizeof(SpotLightData), spotLightData);

	// Fill Global Light Buffer

	globalLightData->numberOfAmbientLights = 0;

	registry->view<Transform, AmbientLight>().each([this](auto& transform, auto& ambientLight)
	{
		globalLightData->ambientLights[globalLightData->numberOfAmbientLights].color = Vector4(ambientLight.color.x, ambientLight.color.y, ambientLight.color.z,1);
		globalLightData->ambientLights[globalLightData->numberOfAmbientLights].intensity = ambientLight.intensity;
		globalLightData->numberOfAmbientLights++;
	});

	globalLightData->numberOfDirectionalLights = 0;

	registry->view<Transform, DirectionalLight>().each([this](auto& transform, auto& directionalLight)
	{
		Vector3 forward = transform.GetForward();
		globalLightData->directionalLights[globalLightData->numberOfDirectionalLights].direction = Vector4(forward.x, forward.y, forward.z, 0);
		globalLightData->directionalLights[globalLightData->numberOfDirectionalLights].color = Vector4(directionalLight.color.x, directionalLight.color.y, directionalLight.color.z, 1);
		globalLightData->directionalLights[globalLightData->numberOfDirectionalLights].intensity = directionalLight.intensity;
		globalLightData->numberOfDirectionalLights++;
	});

	globalLightSSBO.Set(3, sizeof(GlobalLightData), globalLightData);

	// Cull Lights

	cullLights->Use();
	cullLights->SetUVector3("clusterSizes", CLUSTERS_X, CLUSTERS_Y, CLUSTERS_Z);
	cullLights->SetInt("numPointLights", numPointLightsInScene);
	cullLights->SetInt("numSpotLights", numSpotLightsInScene);
	cullLights->SetMatrix4x4("view", viewMatrix);
	cullLights->Dispatch(CLUSTERS_X, CLUSTERS_Y, CLUSTERS_Z);
}