/***********************************************************************
 * Module:  TestingMessageServiceFactory.h
 * Author:  milk
 * Modified: 2016年4月29日 19:22:09
 * Purpose: Declaration of the class TestingMessageServiceFactory
 ***********************************************************************/

#if !defined(__TestingServer_TestingMessageServiceFactory_h)
#define __TestingServer_TestingMessageServiceFactory_h

#include "../message/MessageServiceFactory.h"

class TestingMessageServiceFactory : public MessageServiceFactory
{
public:
   std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(IServerContext* serverCtx);
   std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(IServerContext* serverCtx);
   IMessageProcessor_Ptr CreateWorkerProcessor(IServerContext* serverCtx);
   IMessageProcessor_Ptr CreateMessageProcessor(IServerContext* serverCtx);

protected:
private:

};

#endif