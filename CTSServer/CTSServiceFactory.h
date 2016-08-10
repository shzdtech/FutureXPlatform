/***********************************************************************
 * Module:  CTSServiceFactory.h
 * Author:  milk
 * Modified: 2015年11月8日 15:47:20
 * Purpose: Declaration of the class CTSServiceFactory
 ***********************************************************************/

#if !defined(__CTSServer_CTSServiceFactory_h)
#define __CTSServer_CTSServiceFactory_h

#include "../message/MessageServiceFactory.h"

class CTSServiceFactory : public MessageServiceFactory
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