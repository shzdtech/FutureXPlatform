/***********************************************************************
 * Module:  CTPOTCClientServiceFactory.h
 * Author:  milk
 * Modified: 2015年8月1日 20:23:21
 * Purpose: Declaration of the class CTPOTCClientServiceFactory
 ***********************************************************************/

#if !defined(__CTP_CTPOTCClientServiceFactory_h)
#define __CTP_CTPOTCClientServiceFactory_h

#include "../CTPServer/CTPMDServiceFactory.h"

class CTPOTCClientServiceFactory : public CTPMDServiceFactory
{
public:
   std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(void);
   std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(void);
   IMessageProcessor_Ptr CreateMessageProcessor(void);

protected:
private:

};

#endif