#pragma once

#include "../Vendor/entt/entt.hpp"
#include "Cubemap.hpp"
#include "Texture.hpp"
#include "PBRSettings.hpp"
#include "ClusteredSettings.hpp"
#include "Skybox.hpp"

class RenderSystem
{
public:
	RenderSystem(std::shared_ptr<entt::registry> registry);
	void RenderAll(std::shared_ptr<entt::registry> registry);
private:
	ClusteredSettings* clusteredSettings;
	PBRSettings pbrSettings;
	Skybox skybox;
};

