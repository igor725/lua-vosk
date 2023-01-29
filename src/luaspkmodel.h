#ifndef LUAVOSK_SPKMODEL_H
#define LUAVOSK_SPKMODEL_H
#include "luahelp.h"

void *lua_checkspkmodel(lua_State *L, int idx);
void *lua_testspkmodel(lua_State *L, int idx);
void  luavosk_spkmodel(lua_State *L);
#endif
