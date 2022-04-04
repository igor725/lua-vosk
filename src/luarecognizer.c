#include "core.h"
#include "luahelp.h"
#include "voskbridge.h"
#include "luaspkmodel.h"
#include "luarecognizer.h"

void *lua_checkrecog(lua_State *L, int idx) {
	void **ud = luaL_checkudata(L, idx, "vosk_recognizer");
	if(*ud == NULL) {
		luaL_error(L, "Something went wrong");
		return NULL;
	}
	return *ud;
}

int luavosk_newrecognizer(lua_State *L, void *model, void *spkmodel, float rate) {
	void *recogp;
	if(spkmodel != NULL)
		recogp = vlib.recog_newspk(model, rate, spkmodel);
	else
		recogp = vlib.recog_new(model, rate);
	
	if(!recogp) {
		lua_pushnil(L);
		return 1;
	}

	void **ud = lua_newuserdata(L, sizeof(void *));
	luaL_setmetatable(L, "vosk_recognizer");
	*ud = recogp;

	return 1;
}

static int meta_setspk(lua_State *L) {
	vlib.recog_setspk(
		lua_checkrecog(L, 1),
		lua_checkspkmodel(L, 2)	
	);
	return 0;
}

static int meta_push(lua_State *L) {
	size_t len = 0;
	void *recog = lua_checkrecog(L, 1);
	const void *data = luaL_checklstring(L, 2, &len);
	lua_pushboolean(L, vlib.recog_accept(recog, data, (int)len));
	return 1;
}

#ifdef LUAVOSK_HASJIT
static int meta_pushptr(lua_State *L) {
	lua_pushboolean(L, vlib.recog_accept(
		lua_checkrecog(L, 1),
		*(const void **)lua_topointer(L, 2),
		luaL_checkinteger(L, 3)
	));
	return 1;
}
#endif

static int meta_alts(lua_State *L) {
	vlib.recog_alts(
		lua_checkrecog(L, 1),
		luaL_checkinteger(L, 2)
	);
	return 0;
}

static int meta_timings(lua_State *L) {
	vlib.recog_words(
		lua_checkrecog(L, 1),
		lua_toboolean(L, 2)
	);
	return 0;
}

static int meta_nlsml(lua_State *L) {
	lua_pushboolean(L, vlib.recog_nlsml != NULL);
	if(vlib.recog_nlsml)
		vlib.recog_nlsml(
			lua_checkrecog(L, 1),
			lua_toboolean(L, 2)
		);

	return 1;
}

static int meta_result(lua_State *L) {
	voskstr out = vlib.recog_result(
		lua_checkrecog(L, 1)
	);
	if(out == NULL) lua_pushnil(L);
	else lua_pushstring(L, out);
	return 1;
}

static int meta_partial(lua_State *L) {
	voskstr out = vlib.recog_partial(
		lua_checkrecog(L, 1)
	);
	if(out == NULL) lua_pushnil(L);
	else lua_pushstring(L, out);
	return 1;
}

static int meta_final(lua_State *L) {
	voskstr out = vlib.recog_final(
		lua_checkrecog(L, 1)
	);
	if(out == NULL) lua_pushnil(L);
	else lua_pushstring(L, out);
	return 1;
}

static int meta_reset(lua_State *L) {
	vlib.recog_reset(
		lua_checkrecog(L, 1)
	);
	return 0;
}

static int meta_free(lua_State *L) {
	vlib.recog_free(
		lua_checkrecog(L, 1)
	);
	return 0;
}

static const luaL_Reg recogmeta[] = {
	{"setspk", meta_setspk},
	{"push", meta_push},
#	ifdef LUAVOSK_HASJIT
		{"pushptr", meta_pushptr},
#	endif
	{"alts", meta_alts},
	{"timings", meta_timings},
	{"nlsml", meta_nlsml},
	{"result", meta_result},
	{"partial", meta_partial},
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
