#ifndef __message_macro_h
#define __message_macro_h

#include "../common/typedefs.h"
#include "../message/IProcessorBase.h"

#define OnResponseMacro(MSG_ID, SERIAL_ID, ...){\
	param_vector resp_param_vec = {__VA_ARGS__}; \
	OnResponse(MSG_ID, SERIAL_ID, resp_param_vec); \
}

#define OnResponseProcMacro(processor, MSG_ID, SERIAL_ID, ...){\
	param_vector resp_param_vec = {__VA_ARGS__}; \
	processor->OnResponse(MSG_ID, SERIAL_ID, resp_param_vec); \
}

#define AttribPointerCast(processor, key, T) \
	std::static_pointer_cast<T>(processor->getServerContext()->getAttribute(key))

#endif