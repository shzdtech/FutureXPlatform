/***********************************************************************
 * Module:  CTPTradeServiceFactory.cpp
 * Author:  milk
 * Modified: 2015年3月8日 13:42:10
 * Purpose: Implementation of the class CTPTradeServiceFactory
 ***********************************************************************/

#include "CTPTradeServiceFactory.h"
#include "CTPTradeProcessor.h"
#include "CTPTradeWorkerProcessor.h"
#include "ctp_bizhandlers.h"
#include "CTPConstant.h"

#include "../message/EchoMessageHandler.h"
#include "../message/EchoMessageSerializer.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"

#include "../configuration/AbstractConfigReaderFactory.h"
#include "../dataserializer/AbstractDataSerializerFactory.h"


////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeServiceFactory::CreateMessageHandlers()
// Purpose:    Implementation of CTPTradeServiceFactory::CreateMessageHandlers()
// Return:     std::map<uint, IMessageHandler_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> CTPTradeServiceFactory::CreateMessageHandlers(IServerContext* serverCtx)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map;

	msg_hdl_map[MSG_ID_ECHO] = std::make_shared<EchoMessageHandler>();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<CTPTradeLoginHandler>();;

	msg_hdl_map[MSG_ID_QUERY_ACCOUNT_INFO] = std::make_shared<CTPQueryAccountInfo>();

	msg_hdl_map[MSG_ID_QUERY_EXCHANGE] = std::make_shared<CTPQueryExchange>();

	msg_hdl_map[MSG_ID_QUERY_INSTRUMENT] = std::make_shared<CTPQueryInstrument>();

	msg_hdl_map[MSG_ID_ORDER_NEW] = std::make_shared<CTPNewOrder>();

	msg_hdl_map[MSG_ID_ORDER_CANCEL] = std::make_shared<CTPCancelOrder>();

	msg_hdl_map[MSG_ID_QUERY_ORDER] = std::make_shared<CTPQueryOrder>();

	msg_hdl_map[MSG_ID_ORDER_UPDATE] = std::make_shared<CTPOrderUpdated>();

	msg_hdl_map[MSG_ID_QUERY_POSITION] = std::make_shared<CTPQueryPosition>();

	msg_hdl_map[MSG_ID_QUERY_TRADE] = std::make_shared<CTPQueryTrade>();

	msg_hdl_map[MSG_ID_TRADE_RTN] = std::make_shared<CTPTradeUpdated>();

	msg_hdl_map[MSG_ID_SETTLEMENT_INFO_CONFIRM] = std::make_shared<CTPQuerySettlementInfoCfm>();

	msg_hdl_map[MSG_ID_QUERY_TRANSFER_BANK] = std::make_shared<CTPQueryTransferBank>();

	msg_hdl_map[MSG_ID_QUERY_USER_BANKS] = std::make_shared<CTPQueryUserRegAccount>();
		
	msg_hdl_map[MSG_ID_QUERY_TRANSFER_SERIAL] = std::make_shared<CTPQueryTransferSerial>();
		
	msg_hdl_map[MSG_ID_REQ_BANK_TO_FUTURE] = std::make_shared<CTPFromBankToFuture>();
	
	msg_hdl_map[MSG_ID_REQ_FUTURE_TO_BANK] = std::make_shared<CTPFromFutureToBank>();
	
	msg_hdl_map[MSG_ID_RET_BANK_TO_FUTURE] = std::make_shared<CTPTransferUpdated>();
		
	msg_hdl_map[MSG_ID_RET_FUTURE_TO_BANK] = msg_hdl_map[MSG_ID_RET_BANK_TO_FUTURE];

	msg_hdl_map[MSG_ID_QUERY_USER_BANKACCOUNT] = std::make_shared<CTPQueryUserBankAccount>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of CTPTradeServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> CTPTradeServiceFactory::CreateDataSerializers(IServerContext* serverCtx)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of CTPTradeServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPTradeServiceFactory::CreateMessageProcessor(IServerContext* serverCtx)
{
	auto ret = std::make_shared<CTPTradeProcessor>();
	ret->Initialize(serverCtx);
	return ret;
}


IMessageProcessor_Ptr CTPTradeServiceFactory::CreateWorkerProcessor(IServerContext* serverCtx)
{
	if (!serverCtx->getWorkerProcessor())
	{
		auto worker_ptr = std::make_shared<CTPTradeWorkerProcessor>(serverCtx);
		worker_ptr->Initialize(serverCtx);
		serverCtx->setWorkerProcessor(worker_ptr);
	}

	return serverCtx->getWorkerProcessor();
}