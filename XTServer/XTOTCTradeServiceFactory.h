/***********************************************************************
 * Module:  XTOTCTradeServiceFactory.h
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Declaration of the class XTOTCTradeServiceFactory
 ***********************************************************************/

#if !defined(__XTOTC_CTPOTCTradeServiceFactory_h)
#define __XTOTC_CTPOTCTradeServiceFactory_h

#include "../CTPOTCServer/CTPOTCTradeServiceFactory.h"
#include "xt_export.h"

class XT_CLASS_EXPORT XTOTCTradeServiceFactory : public CTPOTCTradeServiceFactory
{
public:
	std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(IServerContext* serverCtx);
	IMessageProcessor_Ptr CreateWorkerProcessor(IServerContext* serverCtx);
	IMessageProcessor_Ptr CreateMessageProcessor(IServerContext* serverCtx);

protected:
private:

};

#endif