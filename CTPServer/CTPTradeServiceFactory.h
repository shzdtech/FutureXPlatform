/***********************************************************************
 * Module:  CTPTradeServiceFactory.h
 * Author:  milk
 * Modified: 2015年3月8日 13:42:10
 * Purpose: Declaration of the class CTPTradeServiceFactory
 ***********************************************************************/

#if !defined(__CTP_CTPTradeServiceFactory_h)
#define __CTP_CTPTradeServiceFactory_h

#include "../message/MessageServiceFactory.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPTradeServiceFactory : public MessageServiceFactory
{
public:
   std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(IServerContext* serverCtx);
   std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(IServerContext* serverCtx);
   IMessageProcessor_Ptr CreateMessageProcessor(IServerContext* serverCtx);
   IMessageProcessor_Ptr CreateWorkerProcessor(IServerContext* serverCtx);

   bool Load(const std::string& configFile, const std::string& param);

protected:

private:


};

#endif