#if !defined(__CTSServer_CTSQueryAccountInfo_h)
#define __CTSServer_CTSQueryAccountInfo_h

#include "../message/IMessageHandler.h"

class CTSQueryAccountInfo : public IMessageHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif#pragma once
