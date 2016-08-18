/***********************************************************************
 * Module:  AdminMessageServiceFactory.h
 * Author:  milk
 * Modified: 2016年5月14日 22:09:43
 * Purpose: Declaration of the class AdminMessageServiceFactory
 ***********************************************************************/

#if !defined(__AdminServer_AdminMessageServiceFactory_h)
#define __AdminServer_AdminMessageServiceFactory_h

#include "../message/MessageServiceFactory.h"

class AdminMessageServiceFactory : public MessageServiceFactory
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