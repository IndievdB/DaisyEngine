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
	void RenderAll(std::shared_ptr<Texture> renderTexture, std::shared_ptr<Renderbuffer> depthBuffer, std::shared_ptr<Transform> cameraTransform, std::shared_ptr<Camera> camera);
private:
	std::shared_ptr<entt::registry> registry;
	ClusteredSettings* clusteredSettings;
	ShadowSettings* shadowSettings;
	PBRSettings pbrSettings;
	Skybox skybox;
	//
	Framebuffer offscreenFramebuffer;
};

