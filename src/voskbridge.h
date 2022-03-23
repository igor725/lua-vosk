#ifndef LUAVOSK_BRIDGE_H
#define LUAVOSK_BRIDGE_H
struct VoskLib {
	void *lib;

	void  (*set_loglevel)(int);
	void  (*init_gpu)(int);

	void *(*model_new)(const char *);
	void  (*model_free)(void *);

	void *(*recog_new)(void *, float);
	int   (*recog_accept)(void *, void *, int);
	const char *(*recog_final)(void *);
	void  (*recog_reset)(void *);
	void  (*recog_free)(void *);
};

extern struct VoskLib vlib;

int luavosk_initlib(voskstr lib);
int luavosk_ready(void);
#endif
