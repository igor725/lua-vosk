#ifndef LUAVOSK_BRIDGE_H
#define LUAVOSK_BRIDGE_H
#include "main.h"
#include "vosksymbols.h"

extern vstr const LUAVOSK_NL, LUAVOSK_NF;
#define VLIB_TEST_FUNC(FI) if (vlib.FI == NULL) return luaL_error(L, LUAVOSK_NF, #FI)
#define VLIB_TEST_READINESS() if (vlib.lib == NULL) return luaL_error(L, LUAVOSK_NL)
#define VLIB_EXPERIMENTAL() if (_experimental == 0) return luaL_error(L, "Experimental functionality is not enabled")

int luavosk_initlib(vstr lib);
#endif
