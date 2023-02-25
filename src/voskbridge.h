#ifndef LUAVOSK_BRIDGE_H
#define LUAVOSK_BRIDGE_H
#include "main.h"

extern vstr const LUAVOSK_NL, LUAVOSK_NF;
#define VLIB_TEST_FUNC(FI) if (vlib.FI == NULL) return luaL_error(L, LUAVOSK_NF, #FI)
#define VLIB_TEST_READINESS() if (vlib.lib == NULL) return luaL_error(L, LUAVOSK_NL)

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

int luavosk_initlib(vstr lib);
#endif
