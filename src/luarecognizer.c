#include "main.h"
#include "luahelp.h"
#include "luamodel.h"
#include "voskbridge.h"
#include "luaspkmodel.h"
#include "luarecognizer.h"

static const char *metanm[] = {"vosk_recognizer", "vosk_brecognizer"};

vrcg lua_checkrecog(lua_State *L, int idx, int batched) {
	void **ud = luaL_checkudata(L, idx, metanm[batched]);
	if (*ud == NULL) {
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

	if (vlib.recog_newspk != NULL)
		*ud = spkmd ? vlib.recog_newspk(model, rate, spkmd) :
					  vlib.recog_new(model, rate);
	else {
		*ud = vlib.recog_new(model, rate);
		if (spkmd) vlib.recog_setspk(*ud, spkmd);
	}

	if (*ud == NULL)
		return luaL_error(L, "Failed to create a new recognizer");

	luaL_setmetatable(L, metanm[0]);
	return 1;
}

int luavosk_newbrecognizer(lua_State *L) {
	vmdl model = lua_checkmodel(L, 1, 1);
	float rate = (float)luaL_checknumber(L, 2);
	void **ud = lua_newuserdata(L, sizeof(void *));

	if ((*ud = vlib.brecog_new(model, rate)) == NULL)
		return luaL_error(L, "Failed to create a new batched recognizer");

	luaL_setmetatable(L, metanm[1]);
	return 1;
}

static int meta_setspk(lua_State *L) {
	vlib.recog_setspk(
		lua_checkrecog(L, 1, 0),
		lua_checkspkmodel(L, 2)
	);
	return 0;
}

static void addfield(lua_State *L, luaL_Buffer *b, int i) {
	lua_rawgeti(L, 2, i);
	luaL_addchar(b, '"');
	if (lua_type(L, -1) != LUA_TSTRING) {
		luaL_error(L, "Invalid value (%s) at index %d in the grammar table",
			luaL_typename(L, -1), i
		);
		return;
	}
	luaL_addvalue(b);
	luaL_addchar(b, '"');
}

static int meta_setgrm(lua_State *L) {
	VLIB_TEST_FUNC(recog_setgrm);

	vstr grm = NULL;
	if (lua_isstring(L, 2))
		grm = luaL_checkstring(L, 2);
	else if (lua_istable(L, 2)) {
		luaL_Buffer buf;
		luaL_buffinit(L, &buf);
		luaL_addchar(&buf, '[');
		lua_pushnil(L);
		int i, tlen = (int)lua_objlen(L, 2);
		for (i = 1; i < tlen; i++) {
			addfield(L, &buf, i);
			luaL_addchar(&buf, ',');
		}
		if (i == tlen)
			addfield(L, &buf, i);
		luaL_addstring(&buf, "]");
		luaL_pushresult(&buf);
		grm = luaL_checkstring(L, -1);
	}

	vlib.recog_setgrm(lua_checkrecog(L, 1, 0), grm);
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

static int meta_pushptr(lua_State *L) {
	vrcg recog = lua_checkrecog(L, 1, 0);
	const void *data = (const void *)lua_topointer(L, 2);
	lua_Integer size = luaL_checkinteger(L, 3);
	int ret;

	if (lua_toboolean(L, 4))
		ret = vlib.recog_accept_float(recog, (const float *)data, size);
	else
		ret = vlib.recog_accept(recog, data, size);

	lua_pushinteger(L, ret);
	return 1;
}

static int meta_alts(lua_State *L) {
	vlib.recog_alts(
		lua_checkrecog(L, 1, 0),
		luaL_checkinteger(L, 2)
	);
	return 0;
}

static int meta_timings(lua_State *L) {
	vrcg recog = lua_checkrecog(L, 1, 0);
	vlib.recog_words(
		recog,
		lua_toboolean(L, 2)
	);
	if (lua_type(L, 3) == LUA_TBOOLEAN) {
		VLIB_TEST_FUNC(recog_pwords);
		vlib.recog_pwords(
			recog,
			lua_toboolean(L, 3)
		);
	}
	return 0;
}

static int meta_nlsml(lua_State *L) {
	VLIB_TEST_FUNC(recog_nlsml);
	vlib.recog_nlsml(
		lua_checkrecog(L, 1, 0),
		lua_toboolean(L, 2)
	);
	return 0;
}

static int meta_result(lua_State *L) {
	lua_pushstring(L, vlib.recog_result(
		lua_checkrecog(L, 1, 0)
	));
	return 1;
}

static int meta_partial(lua_State *L) {
	lua_pushstring(L, vlib.recog_partial(
		lua_checkrecog(L, 1, 0)
	));
	return 1;
}

static int meta_final(lua_State *L) {
	lua_pushstring(L, vlib.recog_final(
		lua_checkrecog(L, 1, 0)
	));
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
	{"pushptr", meta_pushptr},
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
	VLIB_TEST_FUNC(brecog_accept);
	size_t len = 0;
	const void *data = luaL_checklstring(L, 2, &len);
	vlib.brecog_accept(
		lua_checkrecog(L, 1, 1),
		data, (int)len
	);
	return 0;
}

static int bmeta_pushptr(lua_State *L) {
	VLIB_TEST_FUNC(brecog_accept);
	vlib.brecog_accept(
		lua_checkrecog(L, 1, 0),
		lua_topointer(L, 2),
		luaL_checkinteger(L, 3)
	);
	return 0;
}

static int bmeta_nlsml(lua_State *L) {
	VLIB_TEST_FUNC(brecog_nlsml);
	vlib.brecog_nlsml(
		lua_checkrecog(L, 1, 0),
		lua_toboolean(L, 2)
	);
	return 0;
}

static int bmeta_free(lua_State *L) {
	VLIB_TEST_FUNC(brecog_free);
	vlib.brecog_free(
		lua_checkrecog(L, 1, 1)
	);
	return 0;
}

static int bmeta_finish(lua_State *L) {
	VLIB_TEST_FUNC(brecog_finish);
	vlib.brecog_finish(
		lua_checkrecog(L, 1, 1)
	);
	return 0;
}

static int bmeta_result(lua_State *L) {
	VLIB_TEST_FUNC(brecog_fresult);
	lua_pushstring(L, vlib.brecog_fresult(
		lua_checkrecog(L, 1, 1)
	));
	return 1;
}

static int bmeta_pop(lua_State *L) {
	VLIB_TEST_FUNC(brecog_pop);
	vlib.brecog_pop(
		lua_checkrecog(L, 1, 1)
	);
	return 0;
}

static int bmeta_pending(lua_State *L) {
	VLIB_TEST_FUNC(brecog_pending);
	lua_pushinteger(L, (lua_Integer)vlib.brecog_pending(
		lua_checkrecog(L, 1, 1)
	));
	return 1;
}

static const luaL_Reg brecogmeta[] = {
	{"push", bmeta_push},
	{"pushptr", bmeta_pushptr},
	{"nlsml", bmeta_nlsml},
	{"finish", bmeta_finish},
	{"result", bmeta_result},
	{"pop", bmeta_pop},
	{"pending", bmeta_pending},
	{"__gc", bmeta_free},

	{NULL, NULL}
};

void luavosk_recognizer(lua_State *L) {
	luahelp_newmt(L, metanm[0], recogmeta);
	if (vlib.brecog_new)
		luahelp_newmt(L, metanm[1], brecogmeta);
}
