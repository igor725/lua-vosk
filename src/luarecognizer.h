#ifndef LUAVOSK_RECOGNIZER_H
#define LUAVOSK_RECOGNIZER_H
#include "core.h"
#include "luahelp.h"

void *lua_checkrecog(lua_State *L, int idx);
int luavosk_newrecognizer(lua_State *L, void *model, void *spkmodel, float rate);
void luavosk_recognizer(lua_State *L);
#endif
