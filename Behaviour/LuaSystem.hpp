#pragma once

#include "../Vendor/entt/entt.hpp"
#include "../Core/Transform.hpp"

class lua_State;
class TriggerData;
class LuaBehaviour;

class LuaSystem
{
public:
	LuaSystem(std::shared_ptr<entt::registry> registry);
	~LuaSystem();
	void Intialize();
	void Update();
private:
	static std::shared_ptr<entt::registry> registry;
	std::vector<std::shared_ptr<lua_State*>> luaStates;
	static Transform& GetTransform(entt::entity& entity);
	static bool GetKey(std::string input);
	void RegisterAll(std::shared_ptr<lua_State*> L);
	void ProcessTriggerData(LuaBehaviour behaviour, TriggerData triggerData);
};