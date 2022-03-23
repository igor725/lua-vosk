#include "luahelp.h"
#include "luamodel.h"
#include "voskbridge.h"
#include "luarecognizer.h"

static int loadvosk(lua_State *L) {
	voskstr libname = luaL_checkstring(L, 1);
	if(!luavosk_initlib(libname))
		luaL_error(L, "Failed to load libvosk");
	return 0;
}

static int loglevel(lua_State *L) {
	int level = luaL_checkinteger(L, 1);
	vlib.set_loglevel(level);
	return 0;
}

static int usegpu(lua_State *L) {
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
	luavosk_model(L, -1);
	luavosk_recognizer(L);
	return 1;
}
