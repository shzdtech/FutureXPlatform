/***********************************************************************
 * Module:  CTPOTCTradeServiceFactory.h
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Declaration of the class CTPOTCTradeServiceFactory
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCTradeServiceFactory_h)
#define __CTPOTC_CTPOTCTradeServiceFactory_h

#include "../CTPServer/CTPMDServiceFactory.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCTradeServiceFactory : public CTPMDServiceFactory
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