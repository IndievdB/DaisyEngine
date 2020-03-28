#include "LuaBehaviour.hpp"
#include <iostream>

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#include "../Vendor/LuaBridge/LuaBridge.h"
using namespace luabridge;

void LuaBehaviour::Update()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	lua_pcall(L, 0, 0, 0);

	luaL_dofile(L, scriptPath.c_str());

	lua_close(L);
}