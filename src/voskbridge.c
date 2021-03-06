#include "core.h"
#include "backend.h"
#include "voskbridge.h"

struct VoskLib vlib;
vstr symlist[] = {
	"vosk_set_log_level",
	"vosk_gpu_init",
	"vosk_model_new",
	"vosk_model_find_word",
	"vosk_model_free",
	"vosk_spk_model_new",
	"vosk_spk_model_free",
	"vosk_recognizer_new_spk",
	"vosk_recognizer_set_spk_model",
	"vosk_recognizer_new",
	"vosk_recognizer_set_max_alternatives",
	"vosk_recognizer_set_words",
	"vosk_recognizer_set_nlsml",
	"vosk_recognizer_accept_waveform",
	"vosk_recognizer_result",
	"vosk_recognizer_partial_result",
	"vosk_recognizer_final_result",
	"vosk_recognizer_reset",
	"vosk_recognizer_free",
	NULL
};

int critsyms[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1};

int luavosk_initlib(vstr lib) {
	vlib.lib = backend_loadlib(lib);
	if(vlib.lib == NULL) return 0;

	for(int i = 0; symlist[i]; i++) {
		if(!backend_getsym(vlib.lib, symlist[i], &((void **)&vlib)[i + 1]) && critsyms[i]) {
			backend_unloadlib(vlib.lib);
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
