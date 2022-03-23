#include "core.h"
#include "luahelp.h"
#include "voskbridge.h"
#include "luarecognizer.h"

static void *getrecog(lua_State *L, int idx) {
	void **ud = lua_touserdata(L, idx);
	if(*ud == NULL) {
		luaL_error(L, "Something went wrong");
		return NULL;
	}
	return *ud;
}

int luavosk_newrecognizer(lua_State *L, void *model, float rate) {
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

static int meta_final(lua_State *L) {
	void *recog = getrecog(L, 1);
	voskstr out = vlib.recog_final(recog);
	if(out == NULL)
		lua_pushnil(L);
	else
		lua_pushstring(L, out);
	return 1;
}

static int meta_reset(lua_State *L) {
	void *recog = getrecog(L, 1);
	vlib.recog_reset(recog);
	return 0;
}

static int meta_free(lua_State *L) {
	void *recog = getrecog(L, 1);
	vlib.recog_free(recog);
	return 0;
}

static const luaL_Reg recogmeta[] = {
	{"final", meta_final},
	{"reset", meta_reset},

	{"__gc", meta_free},

	{NULL, NULL}
};

void luavosk_recognizer(lua_State *L) {
	luaL_newmetatable(L, "vosk_recognizer");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, recogmeta, 0);
	lua_pop(L, 1);
}
