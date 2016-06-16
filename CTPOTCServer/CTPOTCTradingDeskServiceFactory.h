/***********************************************************************
 * Module:  CTPOTCTradingDeskServiceFactory.h
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Declaration of the class CTPOTCTradingDeskServiceFactory
 ***********************************************************************/

#if !defined(__CTP_CTPOTCTradingDeskServiceFactory_h)
#define __CTP_CTPOTCTradingDeskServiceFactory_h

#include "../CTPServer/CTPMDServiceFactory.h"

class CTPOTCTradingDeskServiceFactory : public CTPMDServiceFactory
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