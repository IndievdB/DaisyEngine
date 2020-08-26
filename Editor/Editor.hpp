#pragma once

#include "../Vendor/entt/entt.hpp"
#include "../Renderer/RenderSystem.hpp"
#include "../Physics/PhysicsSystem.hpp"
#include "../Behaviour/LuaSystem.hpp"
#include "EditorWindow.hpp"

#include <functional>

class Editor : public std::enable_shared_from_this<Editor>
{
public:
	Editor(std::shared_ptr<entt::registry> registry, std::shared_ptr<RenderSystem> renderer, std::shared_ptr<PhysicsSystem> physicsSystem, std::shared_ptr<LuaSystem> luaSystem);
	~Editor();
	void AddWindows();
	void Update();

	std::vector<std::unique_ptr<EditorWindow>> windows;
	std::shared_ptr<entt::registry> registry;
	std::shared_ptr<RenderSystem> renderer;
	std::shared_ptr<PhysicsSystem> physicsSystem;
	std::shared_ptr<LuaSystem> luaSystem;

	void FocusEntity(entt::entity& entity);
	void FocusResource(std::string resource);
	void CreateEntity();

	std::vector<std::function<void(entt::entity&)>> OnFocusEntity;
	std::vector<std::function<void(std::string)>> OnFocusResource;
};

