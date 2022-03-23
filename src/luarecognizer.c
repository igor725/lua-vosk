#include "core.h"
#include "luahelp.h"
#include "voskbridge.h"
#include "luarecognizer.h"

static void *getrecog(lua_State *L, int idx) {
	void **ud = luaL_checkudata(L, idx, "vosk_recognizer");
	if(*ud == NULL) {
		luaL_error(L, "Something went wrong");
		return NULL;
	}
	return *ud;
}

int luavosk_newrecognizer(lua_State *L, void *model, float rate) {
	void *recogp = vlib.recog_new(model, rate);
	if(!recogp) {
		lua_pushnil(L);
		return 1;
	}

	void **ud = lua_newuserdata(L, sizeof(void *));
	luaL_setmetatable(L, "vosk_recognizer");
	*ud = recogp;

	return 1;
}

static int meta_push(lua_State *L) {
	void *recog = getrecog(L, 1);
	size_t len = 0;
	const void *data = luaL_checklstring(L, 2, &len);
	lua_pushboolean(L, vlib.recog_accept(recog, data, (int)len));
	return 1;
}

#ifdef LUA_JITLIBNAME
static int meta_pushptr(lua_State *L) {
	void *recog = getrecog(L, 1);
	const void *data = lua_topointer(L, 2);
	int len = luaL_checkinteger(L, 3);
	lua_pushboolean(L, vlib.recog_accept(recog, data, len));
	return 1;
}
#endif

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
	{"push", meta_push},
#	ifdef LUA_JITLIBNAME
		{"pushptr", meta_pushptr},
#	endif
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
