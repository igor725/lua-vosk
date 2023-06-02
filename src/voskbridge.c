#include "main.h"
#include "voskbridge.h"
#define VOSK_SYMBOLS_C // Дефайн должен быть строго после загрузки voskbridge.h
#include "vosksymbols.h"

vstr const LUAVOSK_NL = "Please call vosk.init() first";
vstr const LUAVOSK_NF = "There is no such function (vlib.%s) in the loaded vosk library";

#if defined(LUAVOSK_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include "main.h"

#	define __loadlib(_N) LoadLibraryA(_N)
#	define __getsym(_L, _S) (void *)GetProcAddress(_L, _S)
#	define __unloadlib(_L) (void)FreeLibrary(_L)
#elif defined(LUAVOSK_UNIX)
#	include <dlfcn.h>
#	include "main.h"

#	define __loadlib(_N) dlopen(_N, RTLD_LAZY)
#	define __getsym(_L, _S) (void *)dlsym(_L, _S)
#	define __unloadlib(_L) (void)dlclose(_L)
#else
#	error Unknown system
#endif

int luavosk_initlib(vstr lib) {
#	if defined(LUAVOSK_WINDOWS)
		if (sizeof(symlist) != sizeof(vlib)) {
			DebugBreak();
			return 0;
		}
#	endif

	vlib.lib = __loadlib(lib);
	if (vlib.lib == NULL) {
		vlib._invalid = lib;
		return 0;
	}

	for (int i = 0; symlist[i]; i++) {
		vstr name = symlist[i];
		int critical = *name++ == '+';
		if ((((void **)&vlib)[i + 1] = __getsym(vlib.lib, name)) == NULL && critical == 1) {
			__unloadlib(vlib.lib);
			vlib._invalid = name;
			vlib.lib = NULL;
			return 0;
		}
	}

	return 1;
}
