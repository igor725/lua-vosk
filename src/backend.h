#ifndef LUAVOSK_BACKEND_H
#define LUAVOSK_BACKEND_H
#include "core.h"

void *backend_loadlib(vstr lib);
int   backend_getsym(void *lib, vstr symn, void *symp);
void  backend_unloadlib(void *lib);
#endif
