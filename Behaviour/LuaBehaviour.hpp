#pragma once

#include "../Vendor/entt/entt.hpp"
#include "../Physics/TriggerData.hpp"
#include <string>

class lua_State;

struct LuaBehaviour
{
	LuaBehaviour(std::string scriptPath) : scriptPath(scriptPath) {};
	std::string scriptPath;
	std::shared_ptr<lua_State*> L;
};