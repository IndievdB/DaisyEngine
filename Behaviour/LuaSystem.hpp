#pragma once

#include "../Vendor/entt/entt.hpp"
class lua_State;

class LuaSystem
{
public:
	LuaSystem(std::shared_ptr<entt::registry> registry);
	void UpdateAllLuaScripts();
	static std::shared_ptr<entt::registry> registry;
};