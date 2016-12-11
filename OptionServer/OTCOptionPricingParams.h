#if !defined(__OTCOPTION_OTCOptionPricingParams_h)
#define __OTCOPTION_OTCOptionPricingParams_h

#include "../message/NopHandler.h"
#include "otcoption_export.h"

class OPTION_SERVER_CLASS_EXPORT OTCOptionPricingParams : public NopHandler
{
public:
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
};

#endif