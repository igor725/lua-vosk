#ifndef LUAVOSK_BRIDGE_H
#define LUAVOSK_BRIDGE_H
#include "core.h"

#define LUAVOSK_NL "Please call vosk.init() first"

struct VoskLib {
	void  *lib;

	void  (*set_loglevel)(int);
	void  (*init_gpu)(void);

	vmdl  (*model_new)(vstr);
	int   (*model_find)(vmdl, vstr);
	void  (*model_free)(vmdl);

	vsmdl (*spkmodel_new)(vstr);
	void  (*spkmodel_free)(vsmdl);

	vrcg  (*recog_newspk)(vmdl, float, vsmdl);
	void  (*recog_setspk)(vrcg, vsmdl);
	vrcg  (*recog_new)(vmdl, float);
	void  (*recog_alts)(vrcg, int);
	void  (*recog_words)(vrcg, int);
	void  (*recog_nlsml)(vrcg, int);
	int   (*recog_accept)(vrcg, const void *, int);
	vstr  (*recog_result)(vrcg);
	vstr  (*recog_partial)(vrcg);
	vstr  (*recog_final)(vrcg);
	void  (*recog_reset)(vrcg);
	void  (*recog_free)(vrcg);
};

extern struct VoskLib vlib;

int luavosk_initlib(vstr lib);
int luavosk_ready(void);
#endif
