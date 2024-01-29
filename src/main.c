#include "main.h"
#include "luahelp.h"
#include "luamodel.h"
#include "voskbridge.h"
#include "luaspkmodel.h"

int _experimental = 0;

static int loadvosk(lua_State *L) {
	if (lua_istable(L, 1)) {
		int tabsize = (int)lua_rawlen(L, 1);
		for (int i = 1; i <= tabsize; i++) {
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

static lua_Integer _readfile(lua_State *L, void *buff, lua_Integer size, int idx) {
	lua_getmetatable(L, idx);
	lua_getfield(L, -1, "read");
	lua_pushvalue(L, idx);
	lua_pushinteger(L, size);
	lua_call(L, 2, 2);
	if (lua_isnil(L, -2)) {
		lua_pop(L, 3);
		return 0;
	}

	size_t eleng = 0;
	const char *read = luaL_checklstring(L, -2, &eleng);
	char *cbuff = (char *)buff;
	while(eleng--) *cbuff++ = *read++;
	lua_pop(L, 3);
	return (lua_Integer)(cbuff - (char *)buff);
}

static lua_Integer _seekfile(lua_State *L, lua_Integer offset, int origin, int idx) {
	lua_getmetatable(L, idx);
	lua_getfield(L, -1, "seek");
	lua_pushvalue(L, idx);
	switch (origin) {
		case SEEK_CUR:
			lua_pushliteral(L, "cur");
			break;

		case SEEK_END:
			lua_pushliteral(L, "end");
			break;

		case SEEK_SET:
			lua_pushliteral(L, "set");
			break;
	}
	lua_pushinteger(L, offset);
	lua_call(L, 3, 2);
	if (lua_isnil(L, -2)) {
		lua_pop(L, 3);
		return 1;
	}

	lua_pop(L, 3);
	return 0;
}

static int wavguess(lua_State *L) {
	int temp, nfsz, nsamp;
	short nfmt, nchs;

	if (_readfile(L, &temp, 4, 1) != 4 || temp != 0x46464952/*RIFF*/)
		return luaL_error(L, "Not a wave file");
	if (_seekfile(L, 4, SEEK_CUR, 1) != 0 || _readfile(L, &temp, 4, 1) != 4 || temp != 0x45564157/*WAVE*/)
		return luaL_error(L, "WAVE id was not found");
	if (_readfile(L, &temp, 4, 1) != 4 || temp != 0x20746D66/*fmt */)
		return luaL_error(L, "No format sub-chunk found");
	if (_readfile(L, &nfsz, 4, 1) != 4 || nfsz <= 0)
		return luaL_error(L, "Invalid format sub-chunk size");
	if (_readfile(L, &nfmt, 2, 1) != 2 || (nfmt != 0x0001 && nfmt != 0x0003))
		return luaL_error(L, "Unsupported data format");
	if (_readfile(L, &nchs, 2, 1) != 2 || nchs > 1)
		return luaL_error(L, "Not a mono sound file");
	if (_readfile(L, &nsamp, 4, 1) != 4 || nsamp == 0)
		return luaL_error(L, "Failed to determine sample rate");
	if (_seekfile(L, 8, SEEK_CUR, 1) != 0)
		return luaL_error(L, "Unexpected EOF");

	// Читаем расширенные данные для чанка формата, если такие имеются
	if (nfsz > 16) {
		temp = 0;

		if (_readfile(L, &temp, 2, 1) != 2 || _seekfile(L, temp, SEEK_CUR, 1) != 0)
			return luaL_error(L, "Something wrong with the wave extension block");
	}

	while (1) {
		if(_readfile(L, &temp, 4, 1) != 4 || _readfile(L, &nfsz, 4, 1) != 4) // Реюзаем nfsz, т.к. дальше он не нужен
			return luaL_error(L, "Failed to read chunk info");

		if (temp == 0x61746164) // При появлении data чанка завершаем цикл
			break;

		_seekfile(L, nfsz, SEEK_CUR, 1);
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

static int experimental(lua_State *L) {
	_experimental = 1;
	return 0;
}

static luaL_Reg vosklib[] = {
	{"init", loadvosk},
	{"loglevel", loglevel},
	{"wavguess", wavguess},
	{"usegpu", usegpu},
	{"experimental", experimental},

	{NULL, NULL}
};

int LUAVOSK_API luaopen_luavosk(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, vosklib, 0);
	luavosk_spkmodel(L);
	lua_setfield(L, -2, "spkmodel");
	luavosk_model(L);
	lua_setfield(L, -2, "model");
#	ifdef VOSK_ENABLE_JSON
		lua_pushboolean(L, 1);
#	else
		lua_pushboolean(L, 0);
#	endif
	lua_setfield(L, -2, "hasjson");

	return 1;
}
