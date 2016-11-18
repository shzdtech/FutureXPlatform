#ifndef __message_macro_h
#define __message_macro_h

#include "../common/typedefs.h"
#include "TemplateMessageProcessor.h"

#define OnResponseMacro(MSG_ID, SERIAL_ID, ...){\
	OnResponse(MSG_ID, SERIAL_ID, param_vector{__VA_ARGS__}); \
}

#define OnResponseProcMacro(processor, MSG_ID, SERIAL_ID, ...){\
	processor->OnResponse(MSG_ID, SERIAL_ID, param_vector{__VA_ARGS__}); \
}

#define ProcessResponseMacro(processor, MSG_ID, SERIAL_ID, ...){\
	processor->ProcessResponse(MSG_ID, SERIAL_ID, param_vector{__VA_ARGS__}, false); \
}

#define ServerAttribPtrCast(processor, key, T) \
	std::static_pointer_cast<T>(processor->getServerContext()->getAttribute(key))

#endif