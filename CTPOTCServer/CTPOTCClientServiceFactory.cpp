/***********************************************************************
 * Module:  CTPOTCClientServiceFactory.cpp
 * Author:  milk
 * Modified: 2015年8月1日 20:23:21
 * Purpose: Implementation of the class CTPOTCClientServiceFactory
 ***********************************************************************/

#include "CTPOTCClientServiceFactory.h"
#include "ctpotc_bizhandlers.h"
#include "../CTPServer/CTPMarketDataProcessor.h"
#include "CTPOTCWorkerProcessor.h"
#include "../CTPServer/CTPWorkerProcessorID.h"
#include "../CTPServer/ctp_bizhandlers.h"

#include "../message/EchoMsgHandler.h"
#include "../message/EchoMsgSerializer.h"
#include "../message/DefMessageID.h"

#include "../dataobject/AbstractDataSerializerFactory.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCClientServiceFactory::CreateMessageHandlers()
 // Purpose:    Implementation of CTPOTCClientServiceFactory::CreateMessageHandlers()
 // Return:     std::map<uint, IMessageHandler_Ptr>
 ////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> CTPOTCClientServiceFactory::CreateMessageHandlers(void)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map;

	msg_hdl_map[MSG_ID_ECHO] = std::make_shared<EchoMsgHandler>();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<CTPOTCLogin>();

	msg_hdl_map[MSG_ID_SUB_PRICING] = std::make_shared<CTPOTCSubMarketData>();

	msg_hdl_map[MSG_ID_UNSUB_MARKETDATA] = std::make_shared<CTPOTCReturnPricingData>();

	msg_hdl_map[MSG_ID_RTN_PRICING] = std::make_shared<CTPOTCReturnPricingData>();

	msg_hdl_map[MSG_ID_MODIFY_USER_PARAM] = std::make_shared<CTPOTCUpdateUserParam>();

	msg_hdl_map[MSG_ID_QUERY_TRADINGDESK] = std::make_shared<CTPOTCQueryTradingDesks>();

	msg_hdl_map[MSG_ID_ORDER_NEW] = std::make_shared<CTPOTCNewOrder>();

	msg_hdl_map[MSG_ID_ORDER_CANCEL] = std::make_shared<CTPOTCCancelOrder>();

	msg_hdl_map[MSG_ID_QUERY_ORDER] = std::make_shared<CTPOTCQueryOrder>();

	msg_hdl_map[MSG_ID_ORDER_UPDATE] = msg_hdl_map[MSG_ID_QUERY_ORDER];

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCClientServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of CTPOTCClientServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> CTPOTCClientServiceFactory::CreateDataSerializers(void)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCClientServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of CTPOTCClientServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPOTCClientServiceFactory::CreateMessageProcessor(void)
{
	auto ret = std::make_shared<CTPProcessor>(_configMap);
	ret->Initialize();
	return ret;
}