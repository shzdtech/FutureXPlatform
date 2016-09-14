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

#include "../message/EchoMessageHandler.h"
#include "../message/EchoMessageSerializer.h"
#include "../message/DefMessageID.h"
#include "../dataserializer/AbstractDataSerializerFactory.h"

#include "../pricingengine/PricingDataContext.h"

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

	msg_hdl_map[MSG_ID_RTN_PRICING] = std::make_shared<OTCReturnPricingData>();

	msg_hdl_map[MSG_ID_MODIFY_USER_PARAM] = std::make_shared<OTCUpdateUserParam>();

	msg_hdl_map[MSG_ID_QUERY_TRADINGDESK] = std::make_shared<OTCQueryTradingDesks>();

	msg_hdl_map[MSG_ID_ORDER_NEW] = std::make_shared<OTCNewOrder>();

	msg_hdl_map[MSG_ID_ORDER_CANCEL] = std::make_shared<OTCCancelOrder>();

	msg_hdl_map[MSG_ID_QUERY_ORDER] = std::make_shared<OTCQueryOrder>();

	msg_hdl_map[MSG_ID_ORDER_UPDATE] = msg_hdl_map[MSG_ID_QUERY_ORDER];

	msg_hdl_map[MSG_ID_QUERY_STRATEGY] = std::make_shared<OTCQueryStrategy>();

	msg_hdl_map[MSG_ID_QUERY_CONTRACT_PARAM] = std::make_shared<OTCQueryContractParam>();

	msg_hdl_map[MSG_ID_MODIFY_CONTRACT_PARAM] = std::make_shared<OTCUpdateContractParam>();

	msg_hdl_map[MSG_ID_MODIFY_STRATEGY] = std::make_shared<OTCUpdateStrategy>();

	msg_hdl_map[MSG_ID_QUERY_INSTRUMENT] = std::make_shared<OTCQueryInstrument>();

	msg_hdl_map[MSG_ID_QUERY_TRADE] = std::make_shared<OTCQueryTrade>();

	msg_hdl_map[MSG_ID_QUERY_PORTFOLIO] = std::make_shared<OTCQueryPortfolio>();

	msg_hdl_map[MSD_ID_PORTFOLIO_NEW] = std::make_shared<OTCNewPortfolio>();

	msg_hdl_map[MSG_ID_UPDATE_MODELPARAMS] = std::make_shared<OTCUpdateModelParams>();

	msg_hdl_map[MSG_ID_QUERY_MODELPARAMS] = std::make_shared<OTCQueryModelParams>();

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
	auto ret = std::make_shared<CTPOTCSessionProcessor>();
	ret->Initialize();
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
		auto pricingCtx = (IPricingDataContext*)serverCtx->getAttribute(STR_KEY_SERVER_PRICING_DATACONTEXT).get();
		auto tradeProcessor = std::make_shared<CTPOTCTradeProcessor>(serverCtx, pricingCtx);
		tradeProcessor->Initialize(serverCtx);
		auto worker_ptr = std::make_shared<CTPOTCWorkerProcessor>(serverCtx, tradeProcessor);
		worker_ptr->Initialize(serverCtx);
		serverCtx->setWorkerProcessor(worker_ptr);
		serverCtx->setSubTypeWorkerPtr(static_cast<OTCWorkerProcessor*>(worker_ptr.get()));
	}

	return serverCtx->getWorkerProcessor();
}

void CTPOTCServiceFactory::SetServerContext(IServerContext * serverCtx)
{
	CTPMDServiceFactory::SetServerContext(serverCtx);
	serverCtx->setAttribute(STR_KEY_SERVER_PRICING_DATACONTEXT, std::make_shared<PricingDataContext>());
}
