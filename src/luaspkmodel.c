#include "luahelp.h"
#include "voskbridge.h"
#include "luaspkmodel.h"

void *lua_checkspkmodel(lua_State *L, int idx) {
	void **ud = luaL_checkudata(L, idx, "vosk_spkmodel");
	if(*ud == NULL) {
		luaL_error(L, "Something went wrong");
		return NULL;
	}
	return *ud;
}

void *lua_testspkmodel(lua_State *L, int idx) {
	void **ud = luaL_testudata(L, idx, "vosk_spkmodel");
	if(ud == NULL || *ud == NULL) return NULL;
	return *ud;
}

static int meta_free(lua_State *L) {
	vlib.spkmodel_free(lua_checkspkmodel(L, 1));
	return 0;
}

static const luaL_Reg modelmeta[] = {
	{"__gc", meta_free},

	{NULL, NULL}
};

static int newspkmodel(lua_State *L) {
	if(!luavosk_ready()) {
		luaL_error(L, LUAVOSK_NL);
		return 0;
	}

	voskstr modeln = luaL_checkstring(L, 1);
	void *modelp = vlib.spkmodel_new(modeln);
	if(!modelp) {
		lua_pushnil(L);
		return 1;
	}

	void **ud = lua_newuserdata(L, sizeof(void *));
	luaL_setmetatable(L, "vosk_spkmodel");
	*ud = modelp;

	return 1;
}

void luavosk_spkmodel(lua_State *L, int idx) {
	luaL_newmetatable(L, "vosk_spkmodel");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, modelmeta, 0);
	lua_pop(L, 1);
	lua_pushcfunction(L, newspkmodel);
	lua_setfield(L, idx - 1, "spkmodel");
}
