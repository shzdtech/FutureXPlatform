#ifndef __message_marco_h
#define __message_marco_h

#include "../dataobject/data_commondef.h"

#define OnResponseMarco(MSG_ID, ...){\
	ParamVector resp_param_vec = {__VA_ARGS__}; \
	OnResponse(MSG_ID, resp_param_vec); \
		}

#define OnResponseProcMarco(processor, MSG_ID, ...){\
	ParamVector resp_param_vec = {__VA_ARGS__}; \
	processor->OnResponse(MSG_ID, resp_param_vec); \
				}

#endif