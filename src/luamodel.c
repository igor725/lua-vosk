#include "luahelp.h"
#include "luamodel.h"
#include "voskbridge.h"
#include "luarecognizer.h"
#include "luaspkmodel.h"

vmdl lua_checkmodel(lua_State *L, int idx) {
	void **ud = luaL_checkudata(L, idx, "vosk_model");
	if(*ud == NULL) {
		luaL_error(L, "Something went wrong");
		return NULL;
	}
	return (vmdl)*ud;
}

static int meta_recog(lua_State *L) {
	return luavosk_newrecognizer(L,
		lua_checkmodel(L, 1),
		lua_testspkmodel(L, 3),
		(float)luaL_checknumber(L, 2)
	);
}

static int meta_find(lua_State *L) {
	lua_pushinteger(L, vlib.model_find(
		lua_checkmodel(L, 1),
		luaL_checkstring(L, 2)
	));
	return 1;
}

static int meta_free(lua_State *L) {
	vlib.model_free(lua_checkmodel(L, 1));
	return 0;
}

static const luaL_Reg modelmeta[] = {
	{"recognizer", meta_recog},
	{"find", meta_find},

	{"__gc", meta_free},

	{NULL, NULL}
};

static int newmodel(lua_State *L) {
	if(!luavosk_ready()) {
		luaL_error(L, LUAVOSK_NL);
		return 0;
	}

	vstr modeln = luaL_checkstring(L, 1);
	vmdl modelp = vlib.model_new(modeln);
	if(!modelp) {
		lua_pushnil(L);
		return 1;
	}

	void **ud = lua_newuserdata(L, sizeof(void *));
	luaL_setmetatable(L, "vosk_model");
	*ud = modelp;

	return 1;
}

void luavosk_model(lua_State *L, int idx) {
	luaL_newmetatable(L, "vosk_model");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, modelmeta, 0);
	lua_pop(L, 1);
	lua_pushcfunction(L, newmodel);
	lua_setfield(L, idx - 1, "model");
}
