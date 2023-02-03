#include "core.h"
#include "voskbridge.h"

vstr const LUAVOSK_NL = "Please call vosk.init() first";
vstr const LUAVOSK_NF = "There is no such function in the loaded vosk library";

struct VoskLib vlib;
static vstr symlist[] = {
	"+vosk_set_log_level",
	"-vosk_gpu_init",

	"+vosk_model_new",
	"+vosk_model_find_word",
	"+vosk_model_free",

	"-vosk_batch_model_new",
	"-vosk_batch_model_free",
	"-vosk_batch_model_wait",

	"-vosk_spk_model_new",
	"-vosk_spk_model_free",

	"-vosk_recognizer_new_spk",
	"-vosk_recognizer_set_grm",
	"-vosk_recognizer_set_spk_model",
	"+vosk_recognizer_new",
	"+vosk_recognizer_set_max_alternatives",
	"+vosk_recognizer_set_words",
	"+vosk_recognizer_set_partial_words",
	"-vosk_recognizer_set_nlsml",
	"+vosk_recognizer_accept_waveform",
	"+vosk_recognizer_accept_waveform_f",
	"+vosk_recognizer_result",
	"+vosk_recognizer_partial_result",
	"+vosk_recognizer_final_result",
	"+vosk_recognizer_reset",
	"+vosk_recognizer_free",

	"-vosk_batch_recognizer_new",
	"-vosk_batch_recognizer_free",
	"-vosk_batch_recognizer_accept_waveform",
	"-vosk_batch_recognizer_set_nlsml",
	"-vosk_batch_recognizer_finish_stream",
	"-vosk_batch_recognizer_front_result",
	"-vosk_batch_recognizer_pop",
	"-vosk_batch_recognizer_get_pending_chunks",

	NULL
};

#if defined(LUAVOSK_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include "core.h"

#	define __loadlib(_N) LoadLibraryA(_N)
#	define __getsym(_L, _S) (void *)GetProcAddress(_L, _S)
#	define __unloadlib(_L) (void)FreeLibrary(_L)
#elif defined(LUAVOSK_UNIX)
#	include <dlfcn.h>
#	include "core.h"

#	define __loadlib(_N) dlopen(_N, RTLD_LAZY)
#	define __getsym(_L, _S) (void *)dlsym(lib, symn)
#	define __unloadlib(_L) (void)dlclose(_L)
#else
#	error Unknown system
#endif

int luavosk_initlib(vstr lib) {
	vlib.lib = __loadlib(lib);
	if(vlib.lib == NULL) return 0;

	for(int i = 0; symlist[i]; i++) {
		vstr name = symlist[i];
		int critical = *name++ == '+';
		if((((void **)&vlib)[i + 1] = __getsym(vlib.lib, name)) == NULL && critical) {
			__unloadlib(vlib.lib);
			vlib.lib = NULL;
			return 0;
		}
	}

	vlib.set_loglevel(-1);
	return 1;
}

int luavosk_ready(void) {
	return vlib.lib != NULL;
}
