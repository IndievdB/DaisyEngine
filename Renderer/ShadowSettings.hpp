#pragma once

#include "../Vendor/entt/entt.hpp"
#include "../Core/Math/Matrix4x4.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

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
};