/***********************************************************************
 * Module:  CTPAccountTradeServiceFactory.cpp
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Implementation of the class CTPAccountTradeServiceFactory
 ***********************************************************************/

#include "CTPAccountTradeServiceFactory.h"
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

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPAccountTradeServiceFactory::CreateMessageHandlers()
 // Purpose:    Implementation of CTPAccountTradeServiceFactory::CreateMessageHandlers()
 // Return:     std::map<uint, IMessageHandler_Ptr>
 ////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> CTPAccountTradeServiceFactory::CreateMessageHandlers(IServerContext* serverCtx)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map =
		CTPTradeServiceFactory::CreateMessageHandlers(serverCtx);

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<CTPAccountLogin>();
	msg_hdl_map[MSG_ID_QUERY_POSITION] = std::make_shared<CTPAccountQueryPosition>();
	msg_hdl_map[MSG_ID_QUERY_POSITION_DIFFER] = std::make_shared<CTPQueryPositionDiffer>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPAccountTradeServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of CTPAccountTradeServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> CTPAccountTradeServiceFactory::CreateDataSerializers(IServerContext* serverCtx)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPAccountTradeServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of CTPAccountTradeServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPAccountTradeServiceFactory::CreateMessageProcessor(IServerContext* serverCtx)
{
	auto pWorker = std::static_pointer_cast<CTPTradeWorkerSAProcessor>(serverCtx->getWorkerProcessor());
	return std::make_shared<CTPProcessor>(pWorker->RawAPI_Ptr());
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPAccountTradeServiceFactory::CreateWorkerProcessor()
// Purpose:    Implementation of CTPAccountTradeServiceFactory::CreateWorkerProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPAccountTradeServiceFactory::CreateWorkerProcessor(IServerContext* serverCtx)
{
	if (!serverCtx->getWorkerProcessor())
	{
		auto worker_ptr = std::make_shared<CTPTradeWorkerSAProcessor>(serverCtx, std::make_shared<PricingDataContext>());
		worker_ptr->Initialize(serverCtx);
		serverCtx->setWorkerProcessor(worker_ptr);
	}

	return serverCtx->getWorkerProcessor();
}


void CTPAccountTradeServiceFactory::SetServerContext(IServerContext * serverCtx)
{
	CTPTradeServiceFactory::SetServerContext(serverCtx);
	//serverCtx->setAttribute(STR_KEY_SERVER_PRICING_DATACONTEXT, std::make_shared<PricingDataContext>());
}
