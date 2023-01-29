#include "luahelp.h"

void luahelp_newmt(lua_State *L, vstr name, const luaL_Reg *funcs) {
	luaL_newmetatable(L, name);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, funcs, 0);
	lua_pop(L, 1);
}
