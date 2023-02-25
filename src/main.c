#include "main.h"
#include "luahelp.h"
#include "luamodel.h"
#include "voskbridge.h"
#include "luaspkmodel.h"

static int loadvosk(lua_State *L) {
	if (lua_istable(L, 1)) {
		size_t tabsize = lua_rawlen(L, 1), i;
		for (i = 1; i <= tabsize; i++) {
			lua_rawgeti(L, 1, i);
			if (lua_isstring(L, -1) && luavosk_initlib(luaL_checkstring(L, -1)))
				return 0;
		}
	} else {
		vstr libname = luaL_optstring(L, 1, LUAVOSK_LIB);
		if (luavosk_initlib(libname)) return 0;
	}

	return luaL_error(L, "Failed to initialize vosk library: %s was not found", vlib._invalid);
}

static int loglevel(lua_State *L) {
	VLIB_TEST_READINESS();
	VLIB_TEST_FUNC(set_loglevel);
	int level = (int)luaL_checkinteger(L, 1);
	vlib.set_loglevel(level);
	return 0;
}

static int usegpu(lua_State *L) {
	VLIB_TEST_READINESS();
	VLIB_TEST_FUNC(init_gpu);
	vlib.init_gpu();
	return 0;
}

static luaL_Reg vosklib[] = {
	{"init", loadvosk},
	{"loglevel", loglevel},
	{"usegpu", usegpu},

	{NULL, NULL}
};

int luaopen_vosk(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, vosklib, 0);
	luavosk_spkmodel(L);
	lua_setfield(L, -2, "spkmodel");
	luavosk_model(L);
	lua_setfield(L, -2, "model");
	return 1;
}
