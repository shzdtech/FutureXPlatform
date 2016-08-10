/***********************************************************************
 * Module:  CTPMDServiceFactory.h
 * Author:  milk
 * Modified: 2014年10月7日 0:02:09
 * Purpose: Declaration of the class CTPMDServiceFactory
 ***********************************************************************/

#if !defined(__CTP_CTPMDServiceFactory_h)
#define __CTP_CTPMDServiceFactory_h

#include "../message/MessageServiceFactory.h"
#include "../message/IMessageProcessor.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPMDServiceFactory : public MessageServiceFactory
{
public:
   std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(IServerContext* serverCtx);
   std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(IServerContext* serverCtx);
   IMessageProcessor_Ptr CreateMessageProcessor(IServerContext* serverCtx);
   bool Load(const std::string& configFile, const std::string& param);

protected:

private:

};

#endif