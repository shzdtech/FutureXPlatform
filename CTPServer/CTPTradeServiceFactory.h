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
   std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(void);
   std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(void);
   IMessageProcessor_Ptr CreateMessageProcessor(void);
   std::map<std::string, IProcessorBase_Ptr> CreateWorkProcessor(void);

   bool Load(const std::string& configFile, const std::string& param);

protected:

private:


};

#endif