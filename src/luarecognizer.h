#ifndef LUAVOSK_RECOGNIZER_H
#define LUAVOSK_RECOGNIZER_H
#include "core.h"
#include "luahelp.h"

int luavosk_newrecognizer(lua_State *L, void *model, float rate);
void luavosk_recognizer(lua_State *L);
#endif
