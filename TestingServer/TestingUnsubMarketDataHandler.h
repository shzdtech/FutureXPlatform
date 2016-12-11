/***********************************************************************
 * Module:  TestingUnsubMarketDataHandler.h
 * Author:  milk
 * Modified: 2016年4月29日 19:25:47
 * Purpose: Declaration of the class TestingUnsubMarketDataHandler
 ***********************************************************************/

#if !defined(__TestingServer_TestingUnsubMarketDataHandler_h)
#define __TestingServer_TestingUnsubMarketDataHandler_h

#include "../message/NopHandler.h"

class TestingUnsubMarketDataHandler : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif