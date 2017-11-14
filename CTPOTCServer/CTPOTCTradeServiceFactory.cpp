/***********************************************************************
 * Module:  CTPOTCTradeServiceFactory.cpp
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Implementation of the class CTPOTCTradeServiceFactory
 ***********************************************************************/

#include "CTPOTCTradeServiceFactory.h"
#include "CTPOTCWorkerProcessor.h"
#include "CTPOTCTradeProcessor.h"

#include "../CTPServer/CTPWorkerProcessorID.h"
#include "../OTCServer/otc_bizhandlers.h"
#include "../CTPServer/ctp_bizhandlers.h"
#include "ctpotc_bizhandlers.h"

#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"

#include "../message/ReturnHandler.h"
#include "../message/EchoMessageHandler.h"
#include "../message/QuerySysParamHandler.h"
#include "../message/EchoMessageSerializer.h"
#include "../message/DefMessageID.h"
#include "../dataserializer/AbstractDataSerializerFactory.h"

#include "../pricingengine/PricingDataContext.h"
#include "../ordermanager/PortfolioPositionContext.h"
#include "../systemsettings/AppContext.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCTradeServiceFactory::CreateMessageHandlers()
// Purpose:    Implementation of CTPOTCTradeServiceFactory::CreateMessageHandlers()
// Return:     std::map<uint, IMessageHandler_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> CTPOTCTradeServiceFactory::CreateMessageHandlers(IServerContext* serverCtx)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map;

	msg_hdl_map[MSG_ID_ECHO] = std::make_shared<EchoMessageHandler>();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<CTPOTCTradeLoginHandler>();

	msg_hdl_map[MSG_ID_ORDER_NEW] = std::make_shared<OTCNewOrder>();

	msg_hdl_map[MSG_ID_ORDER_CANCEL] = std::make_shared<OTCCancelOrder>();

	msg_hdl_map[MSG_ID_QUERY_ORDER] = std::make_shared<OTCQueryOrder>();

	msg_hdl_map[MSG_ID_ORDER_UPDATE] = msg_hdl_map[MSG_ID_QUERY_ORDER];

	// msg_hdl_map[MSG_ID_QUERY_INSTRUMENT] = std::make_shared<OTCQueryInstrument>();

	msg_hdl_map[MSG_ID_QUERY_TRADE] = std::make_shared<OTCQueryTrade>();

	msg_hdl_map[MSG_ID_QUERY_POSITION] = std::make_shared<OTCQueryPosition>();

	msg_hdl_map[MSG_ID_SYNC_POSITION] = std::make_shared<CTPSyncPositionDiffer>();

	msg_hdl_map[MSG_ID_ADD_MANUAL_TRADE] = std::make_shared<CTPAddManualTrade>();

	msg_hdl_map[MSG_ID_QUERY_RISK] = std::make_shared<CTPOTCQueryRisk>();

	msg_hdl_map[MSG_ID_RISK_UPDATED] = std::make_shared<OTCRiskUpdated>();

	msg_hdl_map[MSG_ID_QUERY_VALUATION_RISK] = std::make_shared<CTPOTCQueryValuationRisk>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCTradeServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of CTPOTCTradeServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> CTPOTCTradeServiceFactory::CreateDataSerializers(IServerContext* serverCtx)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCTradeServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of CTPOTCTradeServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPOTCTradeServiceFactory::CreateMessageProcessor(IServerContext* serverCtx)
{
	std::shared_ptr<CTPOTCTradeProcessor> ret(new CTPOTCTradeProcessor());
	ret->Initialize(serverCtx);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCTradeServiceFactory::CreateWorkerProcessor()
// Purpose:    Implementation of CTPOTCTradeServiceFactory::CreateWorkerProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPOTCTradeServiceFactory::CreateWorkerProcessor(IServerContext* serverCtx)
{
	if (!serverCtx->getWorkerProcessor())
	{
		auto positionCtx = std::static_pointer_cast<IUserPositionContext>(AppContext::GetData(STR_KEY_USER_POSITION));
		if (!positionCtx)
		{
			positionCtx = std::make_shared<PortfolioPositionContext>();
			AppContext::SetData(STR_KEY_USER_POSITION, positionCtx);
		}

		auto pricingCtx = std::static_pointer_cast<IPricingDataContext>(serverCtx->getAttribute(STR_KEY_SERVER_PRICING_DATACONTEXT));

		std::string tradeWorker;
		serverCtx->getConfigVal("tradeworker", tradeWorker);

		auto tradeProcessor = std::static_pointer_cast<CTPOTCTradeWorkerProcessor>(GlobalProcessorRegistry::FindProcessor(tradeWorker));

		if (!tradeProcessor)
		{
			tradeProcessor.reset(new CTPOTCTradeWorkerProcessor(serverCtx, pricingCtx, positionCtx));
			ManualOpHub::Instance()->addListener(tradeProcessor);
			tradeProcessor->Initialize(serverCtx);

			GlobalProcessorRegistry::RegisterProcessor(tradeWorker, tradeProcessor);
		}

		serverCtx->setWorkerProcessor(tradeProcessor);
		serverCtx->setSubTypeWorkerPtr(static_cast<CTPOTCTradeWorkerProcessor*>(tradeProcessor.get()));
		serverCtx->setAbstractSubTypeWorkerPtr(static_cast<OTCTradeWorkerProcessor*>(tradeProcessor.get()));
	}

	return serverCtx->getWorkerProcessor();
}

void CTPOTCTradeServiceFactory::SetServerContext(IServerContext * serverCtx)
{
	CTPMDServiceFactory::SetServerContext(serverCtx);
	auto pricingCtx = AppContext::GetData(STR_KEY_SERVER_PRICING_DATACONTEXT);
	if (!pricingCtx)
	{
		pricingCtx = std::make_shared<PricingDataContext>();
		AppContext::SetData(STR_KEY_SERVER_PRICING_DATACONTEXT, pricingCtx);
	}

	serverCtx->setAttribute(STR_KEY_SERVER_PRICING_DATACONTEXT, pricingCtx);
}
