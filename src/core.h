#ifndef LUAVOSK_CORE_H
#define LUAVOSK_CORE_H

#define LUA_LIB

#ifdef _WIN32
#	define LUAVOSK_WINDOWS
#	define LUA_BUILD_AS_DLL
#	define LUAVOSK_LIB "libvosk.dll"
#else
#	define LUAVOSK_UNIX
#	define LUAVOSK_LIB "libvosk.so"
#endif

#ifndef NULL
#	define NULL (void *)0
#endif

typedef const char *vstr;
typedef void *vrcg, *vbrcg, *vmdl, *vbmdl, *vsmdl;
#endif
