/***********************************************************************
 * Module:  CTPAccountTradeServiceFactory.h
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Declaration of the class CTPAccountTradeServiceFactory
 ***********************************************************************/

#if !defined(__CTP_CTPAccountTradeServiceFactory_h)
#define __CTP_CTPAccountTradeServiceFactory_h

#include "../CTPServer/CTPTradeServiceFactory.h"

class CTPAccountTradeServiceFactory : public CTPTradeServiceFactory
{
public:
	void SetServerContext(IContextAttribute* serverCtx);
	std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(void);
	std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(void);
	std::map<std::string, IProcessorBase_Ptr> CreateWorkProcessor(void);
	bool Load(const std::string& configFile, const std::string& param);
	IMessageProcessor_Ptr CreateMessageProcessor(void);

protected:

private:

};

#endif