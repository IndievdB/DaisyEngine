#pragma once

#include "../Vendor/entt/entt.hpp"
#include "../Renderer/RenderSystem.hpp"
#include "../Physics/PhysicsSystem.hpp"
#include "../Behaviour/LuaSystem.hpp"
#include "EditorWindow.hpp"

class Editor
{
public:
	Editor(std::shared_ptr<entt::registry> registry, std::shared_ptr<RenderSystem> renderer, std::shared_ptr<PhysicsSystem> physicsSystem, std::shared_ptr<LuaSystem> luaSystem);
	~Editor();
	void Update();
private:
	std::vector<std::unique_ptr<EditorWindow>> windows;
};

