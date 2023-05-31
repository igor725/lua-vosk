#include "main.h"
#include "luahelp.h"
#include "luamodel.h"
#include "voskbridge.h"
#include "luaspkmodel.h"

static int loadvosk(lua_State *L) {
	if (lua_istable(L, 1)) {
		size_t tabsize = lua_rawlen(L, 1), i;
		for (i = 1; i <= tabsize; i++) {
			lua_rawgeti(L, 1, i);
			if (lua_isstring(L, -1) && luavosk_initlib(luaL_checkstring(L, -1)))
				return 0;
		}
	} else {
		vstr libname = luaL_optstring(L, 1, LUAVOSK_LIB);
		if (luavosk_initlib(libname)) return 0;
	}

	return luaL_error(L, "Failed to initialize vosk library: %s was not found", vlib._invalid);
}

static int loglevel(lua_State *L) {
	VLIB_TEST_READINESS();
	VLIB_TEST_FUNC(set_loglevel);
	int level = (int)luaL_checkinteger(L, 1);
	vlib.set_loglevel(level);
	return 0;
}

static int wavguess(lua_State *L) {
	FILE *fw = *(FILE **)luaL_checkudata(L, 1, LUA_FILEHANDLE);
	int temp, nfsz, nsamp;
	short nfmt, nchs;

	if (fread(&temp, 4, 1, fw) != 1 || temp != 0x46464952/*RIFF*/)
		return luaL_error(L, "Not a wave file");
	if (fseek(fw, 4, SEEK_CUR) != 0 || fread(&temp, 4, 1, fw) != 1 || temp != 0x45564157/*WAVE*/)
		return luaL_error(L, "WAVE id was not found");
	if (fread(&temp, 4, 1, fw) != 1 || temp != 0x20746D66 /*fmt*/)
		return luaL_error(L, "No format sub-chunk found");
	if (fread(&nfsz, 4, 1, fw) != 1 || nfsz <= 0)
		return luaL_error(L, "Invalid format sub-chunk size");
	if (fread(&nfmt, 2, 1, fw) != 1 || (nfmt != 0x0001 && nfmt != 0x0003))
		return luaL_error(L, "Unsupported data format");
	if (fread(&nchs, 2, 1, fw) != 1 || nchs > 1)
		return luaL_error(L, "Not a mono sound file");
	if (fread(&nsamp, 4, 1, fw) != 1 || nsamp == 0)
		return luaL_error(L, "Failed to determine sample rate");
	if (fseek(fw, 8, SEEK_CUR) != 0)
		return luaL_error(L, "Unexpected EOF");

	if (nfsz > 16) {
		temp = 0;

		if (fread(&temp, 2, 1, fw) != 1 || fseek(fw, temp, SEEK_CUR) != 0)
			return luaL_error(L, "Something wrong with the wave extension block");
	}

	while (1) {
		if(fread(&temp, 4, 1, fw) != 1 || fread(&nfsz, 4, 1, fw) != 1) // Реюзаем nfsz, т.к. дальше он не нужен
			return luaL_error(L, "Failed to read chunk info");

		if (temp == 0x61746164)
			break;

		fseek(fw, nfsz, SEEK_CUR);
	}

	lua_pushinteger(L, nsamp);
	lua_pushboolean(L, nfmt == 0x0003);
	return 2;
}

static int usegpu(lua_State *L) {
	VLIB_TEST_READINESS();
	VLIB_TEST_FUNC(init_gpu);
	vlib.init_gpu();
	return 0;
}

static luaL_Reg vosklib[] = {
	{"init", loadvosk},
	{"loglevel", loglevel},
	{"wavguess", wavguess},
	{"usegpu", usegpu},

	{NULL, NULL}
};

int luaopen_vosk(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, vosklib, 0);
	luavosk_spkmodel(L);
	lua_setfield(L, -2, "spkmodel");
	luavosk_model(L);
	lua_setfield(L, -2, "model");
	return 1;
}
