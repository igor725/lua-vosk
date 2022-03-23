#include "core.h"
#include "backend.h"

#if defined(LUAVOSK_WINDOWS)
#	include "backends/windows.c"
#elif defined(LUAVOSK_UNIX)
#	include "backends/unix.c"
#endif
