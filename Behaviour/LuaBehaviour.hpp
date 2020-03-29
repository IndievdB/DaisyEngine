#pragma once

#include "../Vendor/entt/entt.hpp"
#include <string>
class lua_State;

class LuaBehaviour
{
public:
	LuaBehaviour(std::string scriptPath);
	~LuaBehaviour();
	void Update(entt::entity& entity);
private:
	std::string scriptPath;
	lua_State* L;
};