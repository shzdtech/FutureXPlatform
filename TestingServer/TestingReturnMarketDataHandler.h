/***********************************************************************
 * Module:  TestingReturnMarketDataHandler.h
 * Author:  milk
 * Modified: 2016年4月29日 23:13:22
 * Purpose: Declaration of the class TestingReturnMarketDataHandler
 ***********************************************************************/

#if !defined(__TestingServer_TestingReturnMarketDataHandler_h)
#define __TestingServer_TestingReturnMarketDataHandler_h

#include "../message/NopHandler.h"

class TestingReturnMarketDataHandler : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif