#include "luahelp.h"
#include "voskbridge.h"
#include "luaspkmodel.h"

const char *metanm = "vosk_spkmodel";

vsmdl lua_checkspkmodel(lua_State *L, int idx) {
	void **ud = luaL_checkudata(L, idx, metanm);
	if (*ud == NULL) {
		luaL_error(L, "Something went wrong");
		return NULL;
	}
	return (vsmdl)*ud;
}

vsmdl lua_testspkmodel(lua_State *L, int idx) {
	void **ud = luaL_testudata(L, idx, metanm);
	if (ud == NULL || *ud == NULL) return NULL;
	return (vsmdl)*ud;
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
	VLIB_TEST_READINESS();
	VLIB_TEST_FUNC(spkmodel_new);
	vstr modeln = luaL_checkstring(L, 1);
	vsmdl modelp = vlib.spkmodel_new(modeln);
	if (modelp == NULL)
		return luaL_error(L, "Failed to initialize a speaker model");

	void **ud = lua_newuserdata(L, sizeof(void *));
	luaL_setmetatable(L, metanm);
	*ud = modelp;

	return 1;
}

void luavosk_spkmodel(lua_State *L) {
	luaL_newmetatable(L, metanm);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, modelmeta, 0);
	lua_pop(L, 1);
	lua_pushcfunction(L, newspkmodel);
}
