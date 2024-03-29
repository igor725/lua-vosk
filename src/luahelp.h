#ifndef LUAVOSK_HELP_H
#define LUAVOSK_HELP_H
#include "main.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#if LUA_VERSION_NUM == 501
#	define lua_rawlen lua_objlen

#	ifndef LUA_JITLIBNAME
#		define luaL_setmetatable(L, tname) (luaL_getmetatable(L, tname), lua_setmetatable(L, -2))
#		define luaL_setfuncs(L, l, nup) luaL_register(L, NULL, l)

		static inline void *luaL_testudata(lua_State *L, int ud, const char *tname) {
			void *p = lua_touserdata(L, ud);
			if (p != NULL) {
				if (lua_getmetatable(L, ud)) {
					lua_getfield(L, LUA_REGISTRYINDEX, tname);
					if (lua_rawequal(L, -1, -2)) {
						lua_pop(L, 2);
						return p;
					}
				}
			}
			return NULL;
		}
#	endif
#endif

static void inline luahelp_newmt(lua_State *L, vstr name, const luaL_Reg *funcs) {
	luaL_newmetatable(L, name);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, funcs, 0);
	lua_pop(L, 1);
}
#endif
