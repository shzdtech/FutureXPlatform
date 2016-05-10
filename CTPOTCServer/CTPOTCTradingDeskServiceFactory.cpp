/***********************************************************************
 * Module:  CTPOTCTradingDeskServiceFactory.cpp
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Implementation of the class CTPOTCTradingDeskServiceFactory
 ***********************************************************************/

#include "CTPOTCTradingDeskServiceFactory.h"
#include "CTPOTCWorkerProcessor.h"
#include "CTPOTCTradingDeskProcessor.h"

#include "CTPWorkerProcessorID.h"
#include "ctpotc_bizhandlers.h"
#include "../CTPServer/ctp_bizhandlers.h"
#include "../CTPServer/CTPAppContext.h"
#include "../pricingengine/PricingContext.h"

#include "../message/EchoMsgHandler.h"
#include "../message/EchoMsgSerializer.h"
#include "../message/DefMessageID.h"
#include "../dataobject/AbstractMessageSerializerFactory.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCTradingDeskServiceFactory::CreateMessageHandlers()
// Purpose:    Implementation of CTPOTCTradingDeskServiceFactory::CreateMessageHandlers()
// Return:     std::map<uint, IMessageHandler_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> CTPOTCTradingDeskServiceFactory::CreateMessageHandlers(void)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map;

	msg_hdl_map[MSG_ID_ECHO] = std::make_shared<EchoMsgHandler>();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<CTPOTCTradingDeskLogin>();

	msg_hdl_map[MSG_ID_RET_MARKETDATA] = std::make_shared<CTPDepthMarketData>();

	msg_hdl_map[MSG_ID_SUB_PRICING] = std::make_shared<CTPOTCSubMarketData>();

	msg_hdl_map[MSG_ID_RTN_PRICING] = std::make_shared<CTPOTCReturnPricingData>();

	msg_hdl_map[MSG_ID_MODIFY_USER_PARAM] = std::make_shared<CTPOTCUpdateUserParam>();

	msg_hdl_map[MSG_ID_QUERY_STRATEGY] = std::make_shared<CTPOTCQueryStrategy>();

	msg_hdl_map[MSG_ID_QUERY_CONTRACT_PARAM] = std::make_shared<CTPOTCQueryContractParam>();

	msg_hdl_map[MSG_ID_MODIFY_CONTRACT_PARAM] = std::make_shared<CTPOTCUpdateContractParam>();

	msg_hdl_map[MSG_ID_MODIFY_STRATEGY] = std::make_shared<CTPOTCUpdateStrategy>();

	msg_hdl_map[MSG_ID_ORDER_NEW] = std::make_shared<CTPOTCNewOrder>();

	msg_hdl_map[MSG_ID_ORDER_CANCEL] = std::make_shared<CTPOTCCancelOrder>();

	msg_hdl_map[MSG_ID_QUERY_ORDER] = std::make_shared<CTPOTCQueryOrder>();

	msg_hdl_map[MSG_ID_ORDER_UPDATE] = std::make_shared<CTPOTCQueryOrder>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCTradingDeskServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of CTPOTCTradingDeskServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> CTPOTCTradingDeskServiceFactory::CreateDataSerializers(void)
{
	return AbstractMessageSerializerFactory::Instance()->CreateDataSerializers();
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCTradingDeskServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of CTPOTCTradingDeskServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPOTCTradingDeskServiceFactory::CreateMessageProcessor(void)
{
	return std::make_shared<CTPOTCTradingDeskProcessor>(_configMap);
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCTradingDeskServiceFactory::CreateWorkProcessor()
// Purpose:    Implementation of CTPOTCTradingDeskServiceFactory::CreateWorkProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IProcessorBase_Ptr> CTPOTCTradingDeskServiceFactory::CreateWorkProcessor(void)
{
	std::map<uint, IProcessorBase_Ptr> workerProcMap;
	IProcessorBase_Ptr prcPtr;

	if (!(prcPtr = CTPAppContext::FindServerProcessor(WORKPROCESSOR_OTC)))
	{
		prcPtr = std::make_shared<CTPOTCWorkerProcessor>(_configMap);
		CTPAppContext::RegisterServerProcessor(WORKPROCESSOR_OTC, prcPtr);
	}
	workerProcMap[WORKPROCESSOR_OTC] = prcPtr;

	return workerProcMap;
}

bool CTPOTCTradingDeskServiceFactory::Load(const std::string& configFile, const std::string& param)
{
	return CTPMDServiceFactory::Load(configFile, param);
}