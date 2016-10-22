#pragma once
#include "../message/NopHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPOrderUpdated : public NopHandler
{
public:
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);
};

