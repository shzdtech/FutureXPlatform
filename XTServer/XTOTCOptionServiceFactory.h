/***********************************************************************
 * Module:  CTPOTCServiceFactory.h
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Declaration of the class CTPOTCServiceFactory
 ***********************************************************************/

#if !defined(__XTOTC_CTPOTCOptionServiceFactory_h)
#define __XTOTC_CTPOTCOptionServiceFactory_h

#include "../CTPOTCServer/CTPOTCServiceFactory.h"
#include "xt_export.h"

class XT_CLASS_EXPORT XTOTCOptionServiceFactory : public CTPOTCServiceFactory
{
public:
	std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(IServerContext* serverCtx);
	IMessageProcessor_Ptr CreateWorkerProcessor(IServerContext* serverCtx);

protected:
private:

};

#endif