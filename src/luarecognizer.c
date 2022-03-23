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

static const luaL_Reg recogmeta[] = {
	{NULL, NULL}
};

void luavosk_recognizer(lua_State *L) {
	luaL_newmetatable(L, "vosk_recognizer");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, recogmeta, 0);
	lua_pop(L, 1);
}
