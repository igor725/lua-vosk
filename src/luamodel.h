#ifndef LUAVOSK_MODEL_H
#define LUAVOSK_MODEL_H
#include "core.h"
#include "luahelp.h"

vmdl lua_checkmodel(lua_State *L, int idx, int batched);
void luavosk_model(lua_State *L);
#endif
