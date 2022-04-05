#include <dlfcn.h>
#include "core.h"
#include "backend.h"

void *backend_loadlib(vstr path) {
	return dlopen(path, RTLD_LAZY);
}

int backend_getsym(void *lib, vstr symn, void *symp) {
	return (*(void **)symp = (void *)dlsym(lib, symn)) != NULL;
}

void backend_unloadlib(void *lib) {
	(void)dlclose(lib);
}
