/***********************************************************************
 * Module:  CTPOTCServiceFactory.cpp
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Implementation of the class CTPOTCServiceFactory
 ***********************************************************************/

#include "CTPOTCOptionServiceFactory.h"
#include "CTPOTCSessionProcessor.h"
#include "CTPOTCTradeWorkerProcessor.h"
#include "CTPOTCOptionWorkerProcessor.h"

#include "../CTPServer/CTPWorkerProcessorID.h"
#include "../OTCServer/otc_bizhandlers.h"
#include "../OptionServer/otcoption_bizhandlers.h"
#include "../CTPServer/ctp_bizhandlers.h"
#include "../OptionServer/OTCOptionPricingParams.h"
#include "ctpotc_bizhandlers.h"

#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"

#include "../message/EchoMessageHandler.h"
#include "../message/EchoMessageSerializer.h"
#include "../message/DefMessageID.h"
#include "../dataserializer/AbstractDataSerializerFactory.h"

#include "../pricingengine/PricingDataContext.h"
#include "../ordermanager/PortfolioPositionContext.h"
#include "../systemsettings/AppContext.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCServiceFactory::CreateMessageHandlers()
 // Purpose:    Implementation of CTPOTCServiceFactory::CreateMessageHandlers()
 // Return:     std::map<uint, IMessageHandler_Ptr>
 ////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> CTPOTCOptionServiceFactory::CreateMessageHandlers(IServerContext* serverCtx)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map(std::move(CTPOTCServiceFactory::CreateMessageHandlers(serverCtx)));

	msg_hdl_map[MSG_ID_SUB_PRICING] = std::make_shared<OTCOptionSubMarketData>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCServiceFactory::CreateWorkerProcessor()
// Purpose:    Implementation of CTPOTCServiceFactory::CreateWorkerProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPOTCOptionServiceFactory::CreateWorkerProcessor(IServerContext* serverCtx)
{
	if (!serverCtx->getWorkerProcessor())
	{
		std::string tradeWorker;
		serverCtx->getConfigVal("tradeworker", tradeWorker);

		auto tradeProcessor = std::static_pointer_cast<CTPOTCTradeWorkerProcessor>(GlobalProcessorRegistry::FindProcessor(tradeWorker));

		std::shared_ptr<CTPOTCOptionWorkerProcessor> worker_ptr(new CTPOTCOptionWorkerProcessor(serverCtx, tradeProcessor));
		worker_ptr->Initialize(serverCtx);
		serverCtx->setWorkerProcessor(worker_ptr);
		serverCtx->setSubTypeWorkerPtr(static_cast<CTPOTCOptionWorkerProcessor*>(worker_ptr.get()));
		serverCtx->setAbstractSubTypeWorkerPtr(static_cast<OTCWorkerProcessor*>(worker_ptr.get()));
	}

	return serverCtx->getWorkerProcessor();
}