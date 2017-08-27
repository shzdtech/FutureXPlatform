/***********************************************************************
 * Module:  CTPTradeServiceSAFactory.cpp
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Implementation of the class CTPTradeServiceSAFactory
 ***********************************************************************/

#include "CTPTradeServiceSAFactory.h"
#include "CTPWorkerProcessorID.h"
#include "ctp_bizhandlers.h"
#include "CTPTradeWorkerSAProcessor.h"
#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"

#include "../message/EchoMessageHandler.h"
#include "../message/EchoMessageSerializer.h"
#include "../message/DefMessageID.h"

#include "../pricingengine/PricingDataContext.h"
#include "../dataserializer/AbstractDataSerializerFactory.h"
#include "../ordermanager/PortfolioPositionContext.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPTradeServiceSAFactory::CreateMessageHandlers()
 // Purpose:    Implementation of CTPTradeServiceSAFactory::CreateMessageHandlers()
 // Return:     std::map<uint, IMessageHandler_Ptr>
 ////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> CTPTradeServiceSAFactory::CreateMessageHandlers(IServerContext* serverCtx)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map =
		CTPTradeServiceFactory::CreateMessageHandlers(serverCtx);

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<CTPTDLoginSA>();
	msg_hdl_map[MSG_ID_QUERY_POSITION] = std::make_shared<CTPQueryPositionSA>();
	msg_hdl_map[MSG_ID_QUERY_POSITION_DIFFER] = std::make_shared<CTPQueryPositionDiffer>();
	msg_hdl_map[MSG_ID_SYNC_POSITION] = std::make_shared<CTPSyncPositionDiffer>();
	msg_hdl_map[MSG_ID_ADD_MANUAL_TRADE] = std::make_shared<CTPAddManualTrade>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeServiceSAFactory::CreateDataSerializers()
// Purpose:    Implementation of CTPTradeServiceSAFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> CTPTradeServiceSAFactory::CreateDataSerializers(IServerContext* serverCtx)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeServiceSAFactory::CreateMessageProcessor()
// Purpose:    Implementation of CTPTradeServiceSAFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPTradeServiceSAFactory::CreateMessageProcessor(IServerContext* serverCtx)
{
	auto pWorker = std::static_pointer_cast<CTPTradeWorkerSAProcessor>(serverCtx->getWorkerProcessor());
	return std::make_shared<CTPProcessor>(pWorker->RawAPI_Ptr());
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeServiceSAFactory::CreateWorkerProcessor()
// Purpose:    Implementation of CTPTradeServiceSAFactory::CreateWorkerProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPTradeServiceSAFactory::CreateWorkerProcessor(IServerContext* serverCtx)
{
	if (!serverCtx->getWorkerProcessor())
	{
		auto worker_ptr = std::make_shared<CTPTradeWorkerSAProcessor>(serverCtx, 
			std::make_shared<PricingDataContext>(),
			std::make_shared<PortfolioPositionContext>());
		worker_ptr->Initialize(serverCtx);
		serverCtx->setWorkerProcessor(worker_ptr);
		serverCtx->setSubTypeWorkerPtr(static_cast<CTPTradeWorkerSAProcessor*>(worker_ptr.get()));
	}

	return serverCtx->getWorkerProcessor();
}


void CTPTradeServiceSAFactory::SetServerContext(IServerContext * serverCtx)
{
	CTPTradeServiceFactory::SetServerContext(serverCtx);
	//serverCtx->setAttribute(STR_KEY_SERVER_PRICING_DATACONTEXT, std::make_shared<PricingDataContext>());
}
