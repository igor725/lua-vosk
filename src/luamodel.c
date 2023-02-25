#include "luahelp.h"
#include "luamodel.h"
#include "voskbridge.h"
#include "luarecognizer.h"
#include "luaspkmodel.h"

vmdl lua_checkmodel(lua_State *L, int idx, int batched) {
	void **ud = luaL_checkudata(L, idx, batched ? "vosk_bmodel" : "vosk_model");
	if (*ud == NULL) {
		luaL_error(L, "Something went wrong");
		return NULL;
	}
	return (vmdl)*ud;
}

static int meta_find(lua_State *L) {
	lua_pushinteger(L, vlib.model_find(
		lua_checkmodel(L, 1, 0),
		luaL_checkstring(L, 2)
	));
	return 1;
}

static int meta_free(lua_State *L) {
	vlib.model_free(lua_checkmodel(L, 1, 0));
	return 0;
}

static const luaL_Reg modelmeta[] = {
	{"recognizer", luavosk_newrecognizer},
	{"find", meta_find},

	{"__gc", meta_free},

	{NULL, NULL}
};

static int bmeta_wait(lua_State *L) {
	vlib.bmodel_wait(lua_checkmodel(L, 1, 1));
	return 0;
}

static int bmeta_free(lua_State *L) {
	vlib.bmodel_free(lua_checkmodel(L, 1, 1));
	return 0;
}

static const luaL_Reg bmodelmeta[] = {
	{"recognizer", luavosk_newbrecognizer},
	{"wait", bmeta_wait},
	{"__gc", bmeta_free},

	{NULL, NULL}
};

static int newmodel(lua_State *L) {
	VLIB_TEST_READINESS();
	vstr modeln = luaL_checkstring(L, 1);
	void **ud = lua_newuserdata(L, sizeof(void *));
	if (lua_toboolean(L, 2)) {
		if (vlib.bmodel_new == NULL || (*ud = vlib.bmodel_new(modeln)) == NULL)
			return luaL_error(L, "Failed to initialize batched model");
		luaL_setmetatable(L, "vosk_bmodel");
	} else {
		if ((*ud = vlib.model_new(modeln)) == NULL)
			return luaL_error(L, "Failed to initialize model");
		luaL_setmetatable(L, "vosk_model");
	}

	return 1;
}

void luavosk_model(lua_State *L) {
	luavosk_recognizer(L);
	luahelp_newmt(L, "vosk_model", modelmeta);
	if (vlib.bmodel_new)
		luahelp_newmt(L, "vosk_bmodel", bmodelmeta);
	lua_pushcfunction(L, newmodel);
}
