#pragma once

#include "../Vendor/entt/entt.hpp"
#include "../Core/Math/Matrix4x4.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Cubemap.hpp"
#include "CubemapArray.hpp"

class Mesh;
class Skybox;

class ShadowSettings
{
public:
	ShadowSettings(std::shared_ptr<entt::registry> registry);
	void Update(std::shared_ptr<Camera> camera, Matrix4x4& view);
	void Bind(std::shared_ptr<Shader> shader);
private:
	std::shared_ptr<entt::registry> registry;
	Matrix4x4 lightSpaceMatrix;
	Texture* shadowMap;
	void TempDirectionalLight(std::shared_ptr<Camera> camera, Matrix4x4& view);
	void RenderPointLightShadows(std::shared_ptr<Camera> camera, Matrix4x4& view);

	//
public:
	Mesh* cube;
	Shader* cubeShader;
	Skybox* skybox;
	std::unique_ptr<Cubemap> shadowCubeMap;
	std::unique_ptr<CubemapArray> shadowCubeMapArray;
};