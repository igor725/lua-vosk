#include "core.h"
#include "backend.h"

#if defined(LUAVOSK_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include "core.h"
#	include "backend.h"

#	define __loadlib(_N) LoadLibraryA(_N)
#	define __getsym(_L, _S) (void *)GetProcAddress(_L, _S)
#	define __unloadlib(_L) (void)FreeLibrary(_L)
#elif defined(LUAVOSK_UNIX)
#	include <dlfcn.h>
#	include "core.h"
#	include "backend.h"

#	define __loadlib(_N) dlopen(_N, RTLD_LAZY)
#	define __getsym(_L, _S) (void *)dlsym(lib, symn)
#	define __unloadlib(_L) (void)dlclose(_L)
#else
#	error Unknown system
#endif

void *backend_loadlib(vstr path) {
	return __loadlib(path);
}

int backend_getsym(void *lib, vstr symn, void *symp) {
	return (*(void **)symp = __getsym(lib, symn)) != NULL;
}

void backend_unloadlib(void *lib) {
	__unloadlib(lib);
}
