#ifndef LUAVOSK_HELP_H
#define LUAVOSK_HELP_H
#include "core.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#if LUA_VERSION_NUM == 501
#	if defined(LUA_JITLIBNAME)
#		define LUAVOSK_HASJIT
#	else
#		define luaL_setmetatable(L, tname) (luaL_getmetatable(L, tname), lua_setmetatable(L, -2))
#		define luaL_setfuncs(L, l, nup) luaL_register(L, NULL, l)
#	endif
#endif
#endif
