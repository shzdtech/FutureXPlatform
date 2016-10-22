#if !defined(__CTP_CTPTradeUpdated_h)
#define __CTP_CTPTradeUpdated_h

#include "../message/NopHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPTradeUpdated : public NopHandler
{
public:
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);
};

#endif
