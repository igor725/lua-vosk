#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "core.h"
#include "backend.h"

void *backend_loadlib(voskstr path) {
	return LoadLibraryA(path);
}

int backend_getsym(void *lib, voskstr symn, void *symp) {
	return (*(void **)symp = (void *)GetProcAddress(lib, symn)) != NULL;
}

void backend_unloadlib(void *lib) {
	(void)FreeLibrary(lib);
}