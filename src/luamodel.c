#include "luahelp.h"
#include "luamodel.h"
#include "voskbridge.h"
#include "luarecognizer.h"

static void *getmodel(lua_State *L, int idx) {
	void **ud = luaL_checkudata(L, idx, "vosk_model");
	if(*ud == NULL) {
		luaL_error(L, "Something went wrong");
		return NULL;
	}
	return *ud;
}

static int meta_recog(lua_State *L) {
	void *model = getmodel(L, 1);
	float rate = (float)luaL_checknumber(L, 2);
	return luavosk_newrecognizer(L, model, rate);
}

static int meta_find(lua_State *L) {
	void *model = getmodel(L, 1);
	voskstr word = luaL_checkstring(L, 2);
	lua_pushboolean(L, vlib.model_find(model, word));
	return 1;
}

static int meta_free(lua_State *L) {
	vlib.model_free(getmodel(L, 1));
	return 0;
}

static const luaL_Reg modelmeta[] = {
	{"recognizer", meta_recog},
	{"find", meta_find},

	{"__gc", meta_free},

	{NULL, NULL}
};

int luavosk_newmodel(lua_State *L) {
	if(!luavosk_ready()) {
		luaL_error(L, "Please call vosk.init() first");
		return 0;
	}

	voskstr modeln = luaL_checkstring(L, 1);
	void *modelp = vlib.model_new(modeln);
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
	lua_pushcfunction(L, luavosk_newmodel);
	lua_setfield(L, idx - 1, "model");
}