#pragma once

#include "../message/NopHandler.h"
#include "AdminServerExport.h"

class ADMINSERVER_CLASS_EXPORT ChangePasswordHandler : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
};

