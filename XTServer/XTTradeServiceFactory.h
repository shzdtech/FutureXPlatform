/***********************************************************************
 * Module:  CTPTradeServiceFactory.h
 * Author:  milk
 * Modified: 2015年3月8日 13:42:10
 * Purpose: Declaration of the class CTPTradeServiceFactory
 ***********************************************************************/

#if !defined(__XT_CTPTradeServiceFactory_h)
#define __XT_CTPTradeServiceFactory_h

#include "../message/MessageServiceFactory.h"
#include "xt_export.h"

class XT_CLASS_EXPORT CTPTradeServiceFactory : public MessageServiceFactory
{
public:
   std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(IServerContext* serverCtx);
   std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(IServerContext* serverCtx);
   IMessageProcessor_Ptr CreateMessageProcessor(IServerContext* serverCtx);
   IMessageProcessor_Ptr CreateWorkerProcessor(IServerContext* serverCtx);

protected:

private:


};

#endif