#include "luahelp.h"
#include "luamodel.h"
#include "luarecognizer.h"

static int loadvosk(lua_State *L) {
	voskstr libname = luaL_checkstring(L, 1);
	if(!luavosk_initlib(libname))
		luaL_error(L, "Failed to load libvosk");
	return 0;
}

int luaopen_vosk(lua_State *L) {
	lua_newtable(L);
	lua_pushcfunction(L, loadvosk);
	lua_setfield(L, -2, "init");
	luavosk_model(L, -1);
	luavosk_recognizer(L);
	return 1;
}
