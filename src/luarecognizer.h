#ifndef LUAVOSK_RECOGNIZER_H
#define LUAVOSK_RECOGNIZER_H
#include "core.h"
#include "luahelp.h"

vrcg lua_checkrecog(lua_State *L, int idx, int batched);
int  luavosk_newrecognizer(lua_State *L);
int  luavosk_newbrecognizer(lua_State *L);
void luavosk_recognizer(lua_State *L);
#endif
