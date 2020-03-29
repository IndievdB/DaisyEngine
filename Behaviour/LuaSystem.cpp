#include "LuaSystem.hpp"
#include "LuaBehaviour.hpp"

std::shared_ptr<entt::registry> LuaSystem::registry;

LuaSystem::LuaSystem(std::shared_ptr<entt::registry> registry)
{
	LuaSystem::registry = registry;
}

void LuaSystem::UpdateAllLuaScripts()
{
	LuaSystem::registry->view<LuaBehaviour>().each([](auto entity, auto& luaBehaviour)
	{
		luaBehaviour.Update(entity);
	});
}