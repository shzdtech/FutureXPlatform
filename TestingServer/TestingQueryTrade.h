/***********************************************************************
 * Module:  TestingQueryTrade.h
 * Author:  milk
 * Modified: 2015年7月11日 18:55:31
 * Purpose: Declaration of the class TestingQueryTrade
 ***********************************************************************/

#if !defined(__TestingServer_TestingQueryTrade_h)
#define __TestingServer_TestingQueryTrade_h

#include "../message/MessageHandler.h"

class TestingQueryTrade : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif