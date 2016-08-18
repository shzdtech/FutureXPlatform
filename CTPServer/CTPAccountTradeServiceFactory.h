/***********************************************************************
 * Module:  CTPAccountTradeServiceFactory.h
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Declaration of the class CTPAccountTradeServiceFactory
 ***********************************************************************/

#if !defined(__CTP_CTPAccountTradeServiceFactory_h)
#define __CTP_CTPAccountTradeServiceFactory_h

#include "CTPTradeServiceFactory.h"

class CTPAccountTradeServiceFactory : public CTPTradeServiceFactory
{
public:
	void SetServerContext(IServerContext* serverCtx);
	std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(IServerContext* serverCtx);
	std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(IServerContext* serverCtx);
	IMessageProcessor_Ptr CreateWorkerProcessor(IServerContext* serverCtx);
	IMessageProcessor_Ptr CreateMessageProcessor(IServerContext* serverCtx);

protected:

private:

};

#endif