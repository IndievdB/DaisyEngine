#pragma once

#include "../Vendor/entt/entt.hpp"
#include "Cubemap.hpp"
#include "Texture.hpp"
#include "PBRSettings.hpp"
#include "ClusteredSettings.hpp"
#include "ShadowSettings.hpp"
#include "Skybox.hpp"

class RenderSystem
{
public:
	RenderSystem(std::shared_ptr<entt::registry> registry);
	~RenderSystem();
	void RenderAll(std::shared_ptr<entt::registry> registry);
private:
	ClusteredSettings* clusteredSettings;
	ShadowSettings* shadowSettings;
	PBRSettings pbrSettings;
	Skybox skybox;
};

