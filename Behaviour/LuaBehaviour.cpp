#include "LuaBehaviour.hpp"
#include <iostream>
#include "LuaSystem.hpp"

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#include "../Vendor/LuaBridge/LuaBridge.h"
using namespace luabridge;

#include "../Core/Transform.hpp"
#include "../Core/Input.hpp"

static Transform& GetTransform(entt::entity& entity)
{
	return LuaSystem::registry->get<Transform>(entity);
}

static bool GetKey(std::string input)
{
	return Input::GetKey(Input::StringToKeyCode(input));
}


void RegisterAll(lua_State* L)
{
	getGlobalNamespace(L)
		.beginClass < entt::entity >("entity")
		.endClass();

	getGlobalNamespace(L)
		.beginClass < glm::vec3 >("Vector3")
		.addConstructor<void(*)(void)>()
		.addProperty("x", &glm::vec3::x)
		.addProperty("y", &glm::vec3::y)
		.addProperty("z", &glm::vec3::z)
		.endClass();

	getGlobalNamespace(L)
		.beginClass < glm::quat >("Quaternion")
		.addConstructor<void(*)(void)>()
		.addProperty("x", &glm::quat::x)
		.addProperty("y", &glm::quat::y)
		.addProperty("z", &glm::quat::z)
		.addProperty("w", &glm::quat::w)
		.endClass();

	getGlobalNamespace(L)
		.beginClass < Transform >("Transform")
		.addConstructor<void(*)(void)>()
		.addProperty("position", &Transform::position)
		.addProperty("scale", &Transform::scale)
		.addProperty("rotation", &Transform::rotation)
		.endClass();

	getGlobalNamespace(L)
		.addFunction("GetTransform", GetTransform);

	getGlobalNamespace(L)
		.beginNamespace("Input")
		.addFunction("GetKey", GetKey)
		.endNamespace();
}

LuaBehaviour::LuaBehaviour(std::string scriptPath)
{
	this->scriptPath = scriptPath;
	
	L = luaL_newstate();
	luaL_openlibs(L);
	lua_pcall(L, 0, 0, 0);

	RegisterAll(L);

	luaL_dofile(L, scriptPath.c_str());
}

LuaBehaviour::~LuaBehaviour()
{
	lua_close(L);
}

void LuaBehaviour::Update(entt::entity& entity)
{
	//std::cout << (Input::GetKey(KeyCode(84)) == true) << std::endl;
	LuaRef updateFunction = getGlobal(L, "Update");
	updateFunction(&entity);
}