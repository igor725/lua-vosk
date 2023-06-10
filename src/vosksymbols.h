#include "main.h"

#ifdef VOSK_SYMBOLS_C
static vstr const symlist[] = {
	"-vosk_set_log_level",
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
	"-vosk_recognizer_set_partial_words",
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
	NULL, NULL
};

struct VoskLib vlib;
#else
struct VoskLib {
	void  *lib;

	void  (*set_loglevel)(int);
	void  (*init_gpu)(void);
	vmdl  (*model_new)(vstr);
	int   (*model_find)(vmdl, vstr);
	void  (*model_free)(vmdl);
	vbmdl (*bmodel_new)(vstr);
	void  (*bmodel_free)(vbmdl);
	void  (*bmodel_wait)(vbmdl);
	vsmdl (*spkmodel_new)(vstr);
	void  (*spkmodel_free)(vsmdl);
	vrcg  (*recog_newspk)(vmdl, float, vsmdl);
	void  (*recog_setgrm)(vrcg, vstr);
	void  (*recog_setspk)(vrcg, vsmdl);
	vrcg  (*recog_new)(vmdl, float);
	void  (*recog_alts)(vrcg, int);
	void  (*recog_words)(vrcg, int);
	void  (*recog_pwords)(vrcg, int);
	void  (*recog_nlsml)(vrcg, int);
	int   (*recog_accept)(vrcg, const void *, int);
	int   (*recog_accept_float)(vrcg, const float *, int);
	vstr  (*recog_result)(vrcg);
	vstr  (*recog_partial)(vrcg);
	vstr  (*recog_final)(vrcg);
	void  (*recog_reset)(vrcg);
	void  (*recog_free)(vrcg);

	vbrcg (*brecog_new)(vbmdl, float);
	void  (*brecog_free)(vbrcg);
	void  (*brecog_accept)(vbrcg, const void *, int);
	void  (*brecog_nlsml)(vbrcg, int);
	void  (*brecog_finish)(vbrcg);
	vstr  (*brecog_fresult)(vbrcg);
	void  (*brecog_pop)(vbrcg);
	int   (*brecog_pending)(vbrcg);

	vstr _invalid;
};

extern struct VoskLib vlib;
#endif
