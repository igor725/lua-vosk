#include "core.h"
#include "luahelp.h"
#include "luamodel.h"
#include "voskbridge.h"
#include "luaspkmodel.h"
#include "luarecognizer.h"

vrcg lua_checkrecog(lua_State *L, int idx, int batched) {
	void **ud = luaL_checkudata(L, idx, batched ? "vosk_brecognizer" : "vosk_recognizer");
	if(*ud == NULL) {
		luaL_error(L, "Something went wrong");
		return NULL;
	}
	return (vrcg)*ud;
}

int luavosk_newrecognizer(lua_State *L) {
	vmdl model = lua_checkmodel(L, 1, 0);
	vsmdl spkmd = lua_testspkmodel(L, 3);
	float rate = (float)luaL_checknumber(L, 2);
	void **ud = lua_newuserdata(L, sizeof(void *));
	*ud = spkmd ? vlib.recog_newspk(model, rate, spkmd)
				: vlib.recog_new(model, rate);

	if(*ud == NULL)
		return luaL_error(L, "Failed to create new recognizer");

	luaL_setmetatable(L, "vosk_recognizer");
	return 1;
}

int luavosk_newbrecognizer(lua_State *L) {
	vmdl model = lua_checkmodel(L, 1, 1);
	float rate = (float)luaL_checknumber(L, 2);
	void **ud = lua_newuserdata(L, sizeof(void *));

	if((*ud = vlib.brecog_new(model, rate)) == NULL)
		return luaL_error(L, "Failed to create new batched recognizer");

	luaL_setmetatable(L, "vosk_brecognizer");
	return 1;
}

static int meta_setspk(lua_State *L) {
	vlib.recog_setspk(
		lua_checkrecog(L, 1, 0),
		lua_checkspkmodel(L, 2)
	);
	return 0;
}

static int meta_setgrm(lua_State *L) {
	vlib.recog_setgrm(
		lua_checkrecog(L, 1, 0),
		luaL_checkstring(L, 2)
	);
	return 0;
}

static int meta_push(lua_State *L) {
	size_t len = 0;
	vrcg recog = lua_checkrecog(L, 1, 0);
	const void *data = luaL_checklstring(L, 2, &len);
	int ret;
	if (lua_toboolean(L, 3))
		ret = vlib.recog_accept_float(recog, data, (int)len);
	else
		ret = vlib.recog_accept(recog, data, (int)len);
	lua_pushinteger(L, (lua_Integer)ret);
	return 1;
}

#ifdef LUAVOSK_HASJIT
static int meta_pushptr(lua_State *L) {
	lua_pushinteger(L, (lua_Integer)vlib.recog_accept(
		lua_checkrecog(L, 1, 0),
		*(const void **)lua_topointer(L, 2),
		luaL_checkinteger(L, 3)
	));
	return 1;
}
#endif

static int meta_alts(lua_State *L) {
	vlib.recog_alts(
		lua_checkrecog(L, 1, 0),
		luaL_checkinteger(L, 2)
	);
	return 0;
}

static int meta_timings(lua_State *L) {
	vlib.recog_words(
		lua_checkrecog(L, 1, 0),
		lua_toboolean(L, 2)
	);
	return 0;
}

static int meta_nlsml(lua_State *L) {
	if (vlib.recog_nlsml != NULL)
		return luaL_error(L, LUAVOSK_NF);
	vlib.recog_nlsml(
		lua_checkrecog(L, 1, 0),
		lua_toboolean(L, 2)
	);
	return 0;
}

static int meta_result(lua_State *L) {
	vstr out = vlib.recog_result(
		lua_checkrecog(L, 1, 0)
	);
	if(out == NULL) lua_pushnil(L);
	else lua_pushstring(L, out);
	return 1;
}

static int meta_partial(lua_State *L) {
	vstr out = vlib.recog_partial(
		lua_checkrecog(L, 1, 0)
	);
	if(out == NULL) lua_pushnil(L);
	else lua_pushstring(L, out);
	return 1;
}

static int meta_final(lua_State *L) {
	vstr out = vlib.recog_final(
		lua_checkrecog(L, 1, 0)
	);
	if(out == NULL) lua_pushnil(L);
	else lua_pushstring(L, out);
	return 1;
}

static int meta_reset(lua_State *L) {
	vlib.recog_reset(
		lua_checkrecog(L, 1, 0)
	);
	return 0;
}

static int meta_free(lua_State *L) {
	vlib.recog_free(
		lua_checkrecog(L, 1, 0)
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
	{"grammar", meta_setgrm},
	{"timings", meta_timings},
	{"nlsml", meta_nlsml},
	{"result", meta_result},
	{"partial", meta_partial},
	{"final", meta_final},
	{"reset", meta_reset},

	{"__gc", meta_free},

	{NULL, NULL}
};

static int bmeta_push(lua_State *L) {
	size_t len = 0;
	const void *data = luaL_checklstring(L, 2, &len);
	vlib.brecog_accept(
		lua_checkrecog(L, 1, 1),
		data, (int)len
	);
	return 0;
}

#ifdef LUAVOSK_HASJIT
static int bmeta_pushptr(lua_State *L) {
	vlib.brecog_accept(
		lua_checkrecog(L, 1, 0),
		*(const void **)lua_topointer(L, 2),
		luaL_checkinteger(L, 3)
	);
	return 0;
}
#endif

static int bmeta_nlsml(lua_State *L) {
	if (vlib.brecog_nlsml != NULL)
		return luaL_error(L, LUAVOSK_NF);
	vlib.brecog_nlsml(
		lua_checkrecog(L, 1, 0),
		lua_toboolean(L, 2)
	);
	return 0;
}

static int bmeta_free(lua_State *L) {
	vlib.brecog_free(
		lua_checkrecog(L, 1, 1)
	);
	return 0;
}

static int bmeta_finish(lua_State *L) {
	vlib.brecog_finish(
		lua_checkrecog(L, 1, 1)
	);
	return 0;
}

static int bmeta_result(lua_State *L) {
	lua_pushstring(L, vlib.brecog_fresult(
		lua_checkrecog(L, 1, 1)
	));
	return 1;
}

static int bmeta_pop(lua_State *L) {
	vlib.brecog_pop(
		lua_checkrecog(L, 1, 1)
	);
	return 0;
}

static int bmeta_pending(lua_State *L) {
	lua_pushinteger(L, (lua_Integer)vlib.brecog_pending(
		lua_checkrecog(L, 1, 1)
	));
	return 1;
}

static const luaL_Reg brecogmeta[] = {
	{"push", bmeta_push},
#	ifdef LUAVOSK_HASJIT
		{"pushptr", bmeta_pushptr},
#	endif
	{"nlsml", bmeta_nlsml},
	{"finish", bmeta_finish},
	{"result", bmeta_result},
	{"pop", bmeta_pop},
	{"pending", bmeta_pending},
	{"__gc", bmeta_free},

	{NULL, NULL}
};

void luavosk_recognizer(lua_State *L) {
	luahelp_newmt(L, "vosk_recognizer", recogmeta);
	if(vlib.brecog_new)
		luahelp_newmt(L, "vosk_brecognizer", brecogmeta);
}
