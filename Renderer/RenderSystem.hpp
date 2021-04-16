#pragma once

#include "../Vendor/entt/entt.hpp"
#include "Cubemap.hpp"
#include "Texture.hpp"
#include "PBRSettings.hpp"
#include "ShadowSettings.hpp"
#include "Skybox.hpp"
#include "Framebuffer.hpp"
#include "LightSettings.hpp"

class RenderSystem
{
public:
	RenderSystem(std::shared_ptr<entt::registry> registry);
	void RenderAll(std::shared_ptr<Texture> renderTexture, std::shared_ptr<Renderbuffer> depthBuffer, std::shared_ptr<Transform> cameraTransform, std::shared_ptr<Camera> camera);
private:
	std::shared_ptr<entt::registry> registry;
	ShadowSettings* shadowSettings;
	LightSettings* lightSettings;
	PBRSettings pbrSettings;
	Skybox skybox;
	Framebuffer offscreenFramebuffer;
};