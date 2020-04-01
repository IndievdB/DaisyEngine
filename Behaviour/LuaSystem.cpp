#include "LuaSystem.hpp"
#include "LuaBehaviour.hpp"

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#include "../Vendor/LuaBridge/LuaBridge.h"
using namespace luabridge;

#include "../Core/Input.hpp"
#include "../Physics/BoxCollider.hpp"
#include "../Physics/SphereCollider.hpp"
#include "../Physics/PlaneCollider.hpp"

std::shared_ptr<entt::registry> LuaSystem::registry;

LuaSystem::LuaSystem(std::shared_ptr<entt::registry> registry)
{
	LuaSystem::registry = registry;
}

LuaSystem::~LuaSystem()
{
	for (int i = 0; i < luaStates.size(); i++)
	{
		lua_close(*luaStates[i]);
		luaStates[i].reset();
	}

	luaStates.clear();

	registry->view<LuaBehaviour>().each([](auto& behaviour)
	{
		behaviour.L.reset();
	});
}

Transform& LuaSystem::GetTransform(entt::entity& entity)
{
	return LuaSystem::registry->get<Transform>(entity);
}

bool LuaSystem::GetKey(std::string input)
{
	return Input::GetKey(Input::StringToKeyCode(input));
}

void LuaSystem::RegisterAll(std::shared_ptr<lua_State*> L)
{
	getGlobalNamespace(*L)
		.beginClass < entt::entity >("entity")
		.endClass();

	getGlobalNamespace(*L)
		.beginClass < Vector2 >("Vector2")
		.addConstructor<void(*)(void)>()
		.addProperty("x", &Vector2::x)
		.addProperty("y", &Vector2::y)
		.endClass();

	getGlobalNamespace(*L)
		.beginClass < Vector3 >("Vector3")
		.addConstructor<void(*)(void)>()
		.addProperty("x", &Vector3::x)
		.addProperty("y", &Vector3::y)
		.addProperty("z", &Vector3::z)
		.addFunction("__add", &Vector3::operator+=)
		.addFunction("__sub", &Vector3::operator-=)
		.addFunction("__mul", &Vector3::operator*=)
		.addFunction("__unm", &Vector3::operator-)
		.endClass();

	getGlobalNamespace(*L)
		.beginClass < Quaternion >("Quaternion")
		.addConstructor<void(*)(void)>()
		.addProperty("x", &Quaternion::x)
		.addProperty("y", &Quaternion::y)
		.addProperty("z", &Quaternion::z)
		.addProperty("w", &Quaternion::w)
		.addFunction("SetEuler", (void (Quaternion::*)(float, float, float)) &Quaternion::Set)
		.endClass();

	getGlobalNamespace(*L)
		.beginClass < Transform >("Transform")
		.addConstructor<void(*)(void)>()
		.addProperty("position", &Transform::position)
		.addProperty("scale", &Transform::scale)
		.addProperty("rotation", &Transform::rotation)
		.addFunction("GetForward", &Transform::GetForward)
		.addFunction("GetUp", &Transform::GetUp)
		.addFunction("GetRight", &Transform::GetRight)
		.endClass();

	getGlobalNamespace(*L)
		.addFunction("GetTransform", &LuaSystem::GetTransform);

	getGlobalNamespace(*L)
		.beginNamespace("Input")
		.addFunction("GetKey", &LuaSystem::GetKey)
		.addFunction("GetMouseDelta", Input::GetMousePositionDelta)
		.endNamespace();
}

void LuaSystem::Intialize()
{
	registry->view<LuaBehaviour>().each([this](auto& behaviour)
	{
		behaviour.L = std::make_shared<lua_State*>(luaL_newstate());
		luaL_openlibs(*behaviour.L);
		lua_pcall(*behaviour.L, 0, 0, 0);
		RegisterAll(behaviour.L);
		luaL_dofile(*behaviour.L, behaviour.scriptPath.c_str());
		luaStates.push_back(behaviour.L);
	});
}

void LuaSystem::Update()
{
	registry->view<LuaBehaviour>().each([](auto entity, auto& behaviour)
	{
		LuaRef updateFunction = getGlobal(*behaviour.L, "Update");

		if (updateFunction.isFunction())
			updateFunction(&entity);
	});

	registry->view<LuaBehaviour, BoxCollider>().each([this](auto& behaviour, auto& collider)
		{ ProcessTriggerData(behaviour, collider.triggerData); });

	registry->view<LuaBehaviour, SphereCollider>().each([this](auto& behaviour, auto& collider)
		{ ProcessTriggerData(behaviour, collider.triggerData); });

	registry->view<LuaBehaviour, PlaneCollider>().each([this](auto& behaviour, auto& collider)
		{ ProcessTriggerData(behaviour, collider.triggerData); });
}

void LuaSystem::ProcessTriggerData(LuaBehaviour behaviour, TriggerData triggerData)
{
	if (triggerData.IsEmpty())
		return;

	LuaRef onTriggerStayFunction = getGlobal(*behaviour.L, "OnTriggerStay");
	LuaRef onTriggerEnterFunction = getGlobal(*behaviour.L, "OnTriggerEnter");
	LuaRef onTriggerExitFunction = getGlobal(*behaviour.L, "OnTriggerExit");

	if (onTriggerEnterFunction.isFunction())
	{
		for (const auto& collidingEntity : triggerData.currentFrameCollisions)
		{
			if (triggerData.lastFrameCollisions.find(collidingEntity) == triggerData.lastFrameCollisions.end())
			{
				onTriggerEnterFunction(&collidingEntity);
			}
		}
	}

	if (!onTriggerStayFunction.isFunction() && !onTriggerExitFunction.isFunction())
		return;

	for (const auto& collidingEntity : triggerData.lastFrameCollisions)
	{
		if (triggerData.currentFrameCollisions.find(collidingEntity) == triggerData.currentFrameCollisions.end())
		{
			if (onTriggerExitFunction.isFunction())
				onTriggerExitFunction(&collidingEntity);
		}
		else
		{
			if (onTriggerStayFunction.isFunction())
				onTriggerStayFunction(&collidingEntity);
		}
	}
}