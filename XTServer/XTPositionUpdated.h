#pragma once
#include "../message/NopHandler.h"
#include "xt_export.h"

class XT_CLASS_EXPORT XTPositionUpdated : public NopHandler
{
public:
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
};

