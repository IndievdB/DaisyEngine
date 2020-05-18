#pragma once

#include "../Vendor/entt/entt.hpp"
#include "../Renderer/RenderSystem.hpp"
#include "../Physics/PhysicsSystem.hpp"
#include "../Behaviour/LuaSystem.hpp"
#include "EditorWindow.hpp"

class Editor : public std::enable_shared_from_this<Editor>
{
public:
	Editor();
	~Editor();
	void AddWindows(std::shared_ptr<entt::registry> registry, std::shared_ptr<RenderSystem> renderer, std::shared_ptr<PhysicsSystem> physicsSystem, std::shared_ptr<LuaSystem> luaSystem);
	void Update();
	int selectedEntity = 0;
private:
	std::vector<std::unique_ptr<EditorWindow>> windows;
};

