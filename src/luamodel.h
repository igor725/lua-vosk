#ifndef LUAVOSK_MODEL_H
#define LUAVOSK_MODEL_H
#include "luahelp.h"

void *lua_checkmodel(lua_State *L, int idx);
void luavosk_model(lua_State *L, int idx);
#endif
