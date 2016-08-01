/***********************************************************************
 * Module:  CTPOTCClientServiceFactory.h
 * Author:  milk
 * Modified: 2015年8月1日 20:23:21
 * Purpose: Declaration of the class CTPOTCClientServiceFactory
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCClientServiceFactory_h)
#define __CTPOTC_CTPOTCClientServiceFactory_h

#include "../CTPServer/CTPMDServiceFactory.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCClientServiceFactory : public CTPMDServiceFactory
{
public:
   std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(void);
   std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(void);
   IMessageProcessor_Ptr CreateMessageProcessor(void);

protected:
private:

};

#endif