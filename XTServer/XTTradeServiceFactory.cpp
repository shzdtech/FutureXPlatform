/***********************************************************************
 * Module:  XTTradeServiceFactory.cpp
 * Author:  milk
 * Modified: 2015年3月8日 13:42:10
 * Purpose: Implementation of the class XTTradeServiceFactory
 ***********************************************************************/

#include "XTTradeServiceFactory.h"
#include "XTTradeProcessor.h"
#include "XTTradeWorkerProcessor.h"
#include "../CTPServer/ctp_bizhandlers.h"
#include "xt_bizhandlers.h"
#include "XTConstant.h"

#include "../message/EchoMessageHandler.h"
#include "../message/EchoMessageSerializer.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"

#include "../configuration/AbstractConfigReaderFactory.h"
#include "../dataserializer/AbstractDataSerializerFactory.h"


////////////////////////////////////////////////////////////////////////
// Name:       XTTradeServiceFactory::CreateMessageHandlers()
// Purpose:    Implementation of XTTradeServiceFactory::CreateMessageHandlers()
// Return:     std::map<uint, IMessageHandler_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> XTTradeServiceFactory::CreateMessageHandlers(IServerContext* serverCtx)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map;

	msg_hdl_map[MSG_ID_ECHO] = std::make_shared<EchoMessageHandler>();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<XTTradeLoginHandler>();

	msg_hdl_map[MSG_ID_QUERY_ACCOUNT_INFO] = std::make_shared<XTQueryAccountInfo>();

	msg_hdl_map[MSG_ID_ORDER_NEW] = std::make_shared<XTNewOrder>();

	msg_hdl_map[MSG_ID_ORDER_CANCEL] = std::make_shared<XTCancelOrder>();

	msg_hdl_map[MSG_ID_QUERY_ORDER] = std::make_shared<XTQueryOrder>();

	msg_hdl_map[MSG_ID_ORDER_UPDATE] = std::make_shared<XTOrderUpdated>();

	msg_hdl_map[MSG_ID_QUERY_POSITION] = std::make_shared<XTQueryPosition>();

	msg_hdl_map[MSG_ID_POSITION_UPDATED] = std::make_shared<XTPositionUpdated>();

	msg_hdl_map[MSG_ID_QUERY_TRADE] = std::make_shared<XTQueryTrade>();

	msg_hdl_map[MSG_ID_TRADE_RTN] = std::make_shared<XTTradeUpdated>();

	msg_hdl_map[MSG_ID_QUERY_POSITION_DIFFER] = std::make_shared<CTPQueryPositionDiffer>();

	msg_hdl_map[MSG_ID_SYNC_POSITION] = std::make_shared<CTPSyncPositionDiffer>();

	msg_hdl_map[MSG_ID_ADD_MANUAL_TRADE] = std::make_shared<CTPAddManualTrade>();

	msg_hdl_map[MSG_ID_QUERY_TRADE_DIFFER] = std::make_shared<CTPQueryTradeDiff>();

	msg_hdl_map[MSG_ID_SYNC_TRADE] = std::make_shared<CTPSyncTrade>();

	msg_hdl_map[MSG_ID_QUERY_POSITIONPNL] = std::make_shared<CTPQueryPositionPnL>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTTradeServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of XTTradeServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> XTTradeServiceFactory::CreateDataSerializers(IServerContext* serverCtx)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTTradeServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of XTTradeServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr XTTradeServiceFactory::CreateMessageProcessor(IServerContext* serverCtx)
{
	auto ret = std::make_shared<XTTradeProcessor>();
	ret->Initialize(serverCtx);
	return ret;
}


IMessageProcessor_Ptr XTTradeServiceFactory::CreateWorkerProcessor(IServerContext* serverCtx)
{
	if (!serverCtx->getWorkerProcessor())
	{
		auto worker_ptr = std::make_shared<XTTradeWorkerProcessor>(serverCtx);
		ManualOpHub::Instance()->addListener(worker_ptr);
		worker_ptr->Initialize(serverCtx);
		serverCtx->setWorkerProcessor(worker_ptr);
		serverCtx->setSubTypeWorkerPtr(worker_ptr.get());
	}

	return serverCtx->getWorkerProcessor();
}