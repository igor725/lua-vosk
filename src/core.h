#ifndef LUAVOSK_CORE_H
#define LUAVOSK_CORE_H

#ifdef _WIN32
#	define LUAVOSK_WINDOWS
#	define LUA_BUILD_AS_DLL
#	define LUA_LIB
#else
#	define LUAVOSK_UNIX
#endif

#ifndef NULL
#	define NULL (void *)0
#endif

typedef const char *voskstr;
#endif
