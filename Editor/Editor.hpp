#pragma once

#include "../Vendor/entt/entt.hpp"
#include "../Renderer/RenderSystem.hpp"
#include "EditorWindow.hpp"

class Editor
{
public:
	Editor(std::shared_ptr<entt::registry> registry, std::shared_ptr<RenderSystem> renderer);
	~Editor();
	void Update();
private:
	std::shared_ptr<entt::registry> registry;
	std::vector<std::unique_ptr<EditorWindow>> windows;
};

