/***********************************************************************
 * Module:  CTPOTCServiceFactory.h
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Declaration of the class CTPOTCServiceFactory
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCOptionServiceFactory_h)
#define __CTPOTC_CTPOTCOptionServiceFactory_h

#include "CTPOTCServiceFactory.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCOptionServiceFactory : public CTPOTCServiceFactory
{
public:
	std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(IServerContext* serverCtx);
	IMessageProcessor_Ptr CreateWorkerProcessor(IServerContext* serverCtx);

protected:
private:

};

#endif