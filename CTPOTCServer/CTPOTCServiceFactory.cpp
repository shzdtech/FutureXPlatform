/***********************************************************************
 * Module:  CTPOTCServiceFactory.cpp
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Implementation of the class CTPOTCServiceFactory
 ***********************************************************************/

#include "CTPOTCServiceFactory.h"
#include "CTPOTCWorkerProcessor.h"
#include "CTPOTCSessionProcessor.h"

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
// Name:       CTPOTCServiceFactory::CreateMessageHandlers()
// Purpose:    Implementation of CTPOTCServiceFactory::CreateMessageHandlers()
// Return:     std::map<uint, IMessageHandler_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> CTPOTCServiceFactory::CreateMessageHandlers(IServerContext* serverCtx)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map;

	msg_hdl_map[MSG_ID_ECHO] = std::make_shared<EchoMessageHandler>();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<CTPOTCLogin>();

	msg_hdl_map[MSG_ID_SUB_PRICING] = std::make_shared<OTCSubMarketData>();

	msg_hdl_map[MSG_ID_UNSUB_PRICING] = std::make_shared<OTCUnSubMarketData>();

	msg_hdl_map[MSG_ID_SUB_TRADINGDESK_PRICING] = std::make_shared<OTCSubTradingDeskData>();

	msg_hdl_map[MSG_ID_UNSUB_TRADINGDESK_PRICING] = std::make_shared<OTCUnSubTradingDeskData>();

	msg_hdl_map[MSG_ID_RTN_PRICING] = std::make_shared<ReturnHandler<IPricingDO_Ptr>>();

	msg_hdl_map[MSG_ID_MODIFY_USER_PARAM] = std::make_shared<OTCUpdateUserParam>();

	msg_hdl_map[MSG_ID_QUERY_TRADINGDESK] = std::make_shared<OTCQueryTradingDesks>();

	msg_hdl_map[MSG_ID_QUERY_STRATEGY] = std::make_shared<OTCQueryStrategy>();

	msg_hdl_map[MSG_ID_QUERY_CONTRACT_PARAM] = std::make_shared<OTCQueryContractParam>();

	msg_hdl_map[MSG_ID_MODIFY_CONTRACT_PARAM] = std::make_shared<OTCUpdateContractParam>();

	msg_hdl_map[MSG_ID_MODIFY_STRATEGY] = std::make_shared<OTCUpdateStrategy>();

	msg_hdl_map[MSG_ID_QUERY_INSTRUMENT] = std::make_shared<OTCQueryInstrument>();

	msg_hdl_map[MSG_ID_QUERY_PORTFOLIO] = std::make_shared<OTCQueryPortfolio>();

	msg_hdl_map[MSG_ID_PORTFOLIO_NEW] = std::make_shared<OTCNewPortfolio>();

	msg_hdl_map[MSG_ID_MODIFY_PORTFOLIO] = std::make_shared<OTCUpdatePortfolio>();

	msg_hdl_map[MSG_ID_HEDGE_CONTRACT_UPDATE] = std::make_shared<OTCUpdateHedgeContract>();

	msg_hdl_map[MSG_ID_UPDATE_MODELPARAMS] = std::make_shared<OTCUpdateModelParams>();

	msg_hdl_map[MSG_ID_QUERY_MODELPARAMS] = std::make_shared<OTCQueryModelParams>();

	msg_hdl_map[MSG_ID_QUERY_MODELPARAMDEF] = std::make_shared<OTCQueryModelParamDef>();

	msg_hdl_map[MSG_ID_QUERY_SYSPARAMS] = std::make_shared<QuerySysParamHandler>();

	msg_hdl_map[MSG_ID_UPDATE_TEMPMODELPARAMS] = std::make_shared<OTCUpdateTempModelParam>();

	msg_hdl_map[MSG_ID_MODIFY_PRICING_CONTRACT] = std::make_shared<OTCUpdatePricingContract>();

	// For simulation
	msg_hdl_map[MSG_ID_RET_MARKETDATA] = std::make_shared<CTPSimMarketData>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of CTPOTCServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> CTPOTCServiceFactory::CreateDataSerializers(IServerContext* serverCtx)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of CTPOTCServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPOTCServiceFactory::CreateMessageProcessor(IServerContext* serverCtx)
{
	std::shared_ptr<CTPOTCSessionProcessor> ret(new CTPOTCSessionProcessor);
	ret->Initialize(serverCtx);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCServiceFactory::CreateWorkerProcessor()
// Purpose:    Implementation of CTPOTCServiceFactory::CreateWorkerProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPOTCServiceFactory::CreateWorkerProcessor(IServerContext* serverCtx)
{
	if (!serverCtx->getWorkerProcessor())
	{
		std::string tradeWorker;
		serverCtx->getConfigVal("tradeworker", tradeWorker);

		auto tradeProcessor = std::static_pointer_cast<CTPOTCTradeWorkerProcessor>(GlobalProcessorRegistry::FindProcessor(tradeWorker));

		std::shared_ptr<CTPOTCWorkerProcessor> worker_ptr(new CTPOTCWorkerProcessor(serverCtx, tradeProcessor));
		worker_ptr->Initialize(serverCtx);
		serverCtx->setWorkerProcessor(worker_ptr);
		serverCtx->setSubTypeWorkerPtr(static_cast<CTPOTCWorkerProcessor*>(worker_ptr.get()));
		serverCtx->setAbstractSubTypeWorkerPtr(static_cast<OTCWorkerProcessor*>(worker_ptr.get()));
	}

	return serverCtx->getWorkerProcessor();
}

void CTPOTCServiceFactory::SetServerContext(IServerContext * serverCtx)
{
	CTPMDServiceFactory::SetServerContext(serverCtx);
	auto pricingCtx = std::static_pointer_cast<IPricingDataContext>(AppContext::GetData(STR_KEY_SERVER_PRICING_DATACONTEXT));
	if (!pricingCtx)
	{
		pricingCtx = std::make_shared<PricingDataContext>();
		AppContext::SetData(STR_KEY_SERVER_PRICING_DATACONTEXT, pricingCtx);
	}

	serverCtx->setAttribute(STR_KEY_SERVER_PRICING_DATACONTEXT, pricingCtx);

	auto positionCtx = std::static_pointer_cast<IUserPositionContext>(AppContext::GetData(STR_KEY_USER_POSITION));
	if (!positionCtx)
	{
		positionCtx = std::make_shared<PortfolioPositionContext>();
		AppContext::SetData(STR_KEY_USER_POSITION, positionCtx);
	}

	std::string tradeWorker;
	serverCtx->getConfigVal("tradeworker", tradeWorker);

	if (!GlobalProcessorRegistry::FindProcessor(tradeWorker))
	{
		std::shared_ptr<CTPOTCTradeWorkerProcessor> tradeProcessor(new CTPOTCTradeWorkerProcessor(serverCtx, pricingCtx, positionCtx));
		ManualOpHub::Instance()->addListener(tradeProcessor);
		tradeProcessor->Initialize(serverCtx);

		GlobalProcessorRegistry::RegisterProcessor(tradeWorker, tradeProcessor);
	}
}
