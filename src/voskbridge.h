#ifndef LUAVOSK_BRIDGE_H
#define LUAVOSK_BRIDGE_H
#include "core.h"

struct VoskLib {
	void *lib;

	void  (*set_loglevel)(int);
	void  (*init_gpu)(void);

	void *(*model_new)(const char *);
	int   (*model_find)(void *, voskstr);
	void  (*model_free)(void *);

	void *(*recog_new)(void *, float);
	void  (*recog_alts)(void *, int);
	void  (*recog_words)(void *, int);
	int   (*recog_accept)(void *, const void *, int);
	voskstr (*recog_result)(void *);
	voskstr (*recog_partial)(void *);
	voskstr (*recog_final)(void *);
	void  (*recog_reset)(void *);
	void  (*recog_free)(void *);
};

extern struct VoskLib vlib;

int luavosk_initlib(voskstr lib);
int luavosk_ready(void);
#endif
