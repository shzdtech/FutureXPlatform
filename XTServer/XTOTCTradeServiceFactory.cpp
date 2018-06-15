/***********************************************************************
 * Module:  XTOTCTradeServiceFactory.cpp
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Implementation of the class XTOTCTradeServiceFactory
 ***********************************************************************/

#include "XTOTCTradeServiceFactory.h"
#include "../CTPOTCServer/CTPOTCWorkerProcessor.h"
#include "XTOTCTradeProcessor.h"
#include "XTOTCTradeWorkerProcessor.h"

#include "../CTPServer/CTPWorkerProcessorID.h"
#include "../OTCServer/otc_bizhandlers.h"
#include "xt_bizhandlers.h"
#include "../CTPOTCServer/ctpotc_bizhandlers.h"

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
// Name:       XTOTCTradeServiceFactory::CreateMessageHandlers()
// Purpose:    Implementation of XTOTCTradeServiceFactory::CreateMessageHandlers()
// Return:     std::map<uint, IMessageHandler_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> XTOTCTradeServiceFactory::CreateMessageHandlers(IServerContext* serverCtx)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map = CTPOTCTradeServiceFactory::CreateMessageHandlers(serverCtx);

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<XTOTCTradeLoginHandler>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTOTCTradeServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of XTOTCTradeServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr XTOTCTradeServiceFactory::CreateMessageProcessor(IServerContext* serverCtx)
{
	auto ret = std::make_shared<XTOTCTradeProcessor>();
	ret->Initialize(serverCtx);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTOTCTradeServiceFactory::CreateWorkerProcessor()
// Purpose:    Implementation of XTOTCTradeServiceFactory::CreateWorkerProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr XTOTCTradeServiceFactory::CreateWorkerProcessor(IServerContext* serverCtx)
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

		auto tradeProcessor = std::static_pointer_cast<XTOTCTradeWorkerProcessor>(GlobalProcessorRegistry::FindProcessor(tradeWorker));

		if (!tradeProcessor)
		{
			tradeProcessor.reset(new XTOTCTradeWorkerProcessor(serverCtx, pricingCtx, positionCtx));
			ManualOpHub::Instance()->addListener(tradeProcessor);
			tradeProcessor->Initialize(serverCtx);

			GlobalProcessorRegistry::RegisterProcessor(tradeWorker, tradeProcessor);
		}

		serverCtx->setWorkerProcessor(tradeProcessor);
		serverCtx->setSubTypeWorkerPtr(static_cast<XTOTCTradeWorkerProcessor*>(tradeProcessor.get()));
		serverCtx->setAbstractSubTypeWorkerPtr(static_cast<OTCTradeWorkerProcessor*>(tradeProcessor.get()));
	}

	return serverCtx->getWorkerProcessor();
}
