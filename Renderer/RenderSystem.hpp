#pragma once

#include "../Vendor/entt/entt.hpp"
#include "Cubemap.hpp"
#include "Texture.hpp"
#include "PBRSettings.hpp"
#include "ClusteredSettings.hpp"
#include "ShadowSettings.hpp"
#include "Skybox.hpp"
#include "Framebuffer.hpp"

class RenderSystem
{
public:
	RenderSystem(std::shared_ptr<entt::registry> registry);
	void RenderAll(std::shared_ptr<entt::registry> registry);
	std::shared_ptr<Texture> GetOffscreenTexture();
private:
	ClusteredSettings* clusteredSettings;
	ShadowSettings* shadowSettings;
	PBRSettings pbrSettings;
	Skybox skybox;
	//
	Framebuffer offscreenFramebuffer;
	std::shared_ptr<Texture> offscreenTexture;
	std::unique_ptr<Renderbuffer> offscreenDepthRenderbuffer;
};

