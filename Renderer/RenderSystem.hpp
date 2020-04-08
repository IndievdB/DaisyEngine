#pragma once

#include "../Vendor/entt/entt.hpp"
#include "Cubemap.hpp"
#include "Texture.hpp"
#include "PBRSettings.hpp"
#include "Skybox.hpp"

class RenderSystem
{
public:
	RenderSystem();
	void RenderAll(std::shared_ptr<entt::registry> registry);
private:
	PBRSettings pbrSettings;
	Skybox skybox;
	/*std::unique_ptr<Cubemap> irradianceCubemap;
	std::unique_ptr<Cubemap> prefilterCubemap;
	std::unique_ptr<Cubemap> environmentCubemap;
	std::unique_ptr<Texture> brdfLUTTexture;*/
};

