#include "luahelp.h"
#include "luamodel.h"
#include "voskbridge.h"
#include "luarecognizer.h"
#include "luaspkmodel.h"

static int loadvosk(lua_State *L) {
	voskstr libname = luaL_optstring(L, 1, LUAVOSK_LIB);
	if(!luavosk_initlib(libname))
		luaL_error(L, "Failed to load libvosk");
	return 0;
}

static int loglevel(lua_State *L) {
	if(!luavosk_ready()) {
		luaL_error(L, LUAVOSK_NL);
		return 0;
	}

	int level = (int)luaL_checkinteger(L, 1);
	vlib.set_loglevel(level);
	return 0;
}

static int usegpu(lua_State *L) {
	if(!luavosk_ready()) {
		luaL_error(L, LUAVOSK_NL);
		return 0;
	}

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
	luavosk_spkmodel(L, -1);
	luavosk_recognizer(L);
	luavosk_model(L, -1);
	return 1;
}
