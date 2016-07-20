/***********************************************************************
 * Module:  TestingMessageServiceFactory.cpp
 * Author:  milk
 * Modified: 2016年4月29日 19:22:09
 * Purpose: Implementation of the class TestingMessageServiceFactory
 ***********************************************************************/

#include "TestingMessageServiceFactory.h"
#include "TestingServerMessageProcessor.h"
#include "TestingLoginHandler.h"
#include "TestingSubMarketDataHandler.h"
#include "TestingUnsubMarketDataHandler.h"
#include "TestingReturnMarketDataHandler.h"

#include "TestingCreateOrderHandler.h"
#include "TestingCancelOrderHandler.h"
#include "TestingPositionHandler.h"
#include "TestingAccountInfoHandler.h"

#include "../message/EchoMessageHandler.h"
#include "../message/EchoMessageSerializer.h"
#include "../message/DefMessageID.h"
#include "../dataobject/AbstractDataSerializerFactory.h"

////////////////////////////////////////////////////////////////////////
// Name:       TestingMessageServiceFactory::CreateMessageHandlers()
// Purpose:    Implementation of TestingMessageServiceFactory::CreateMessageHandlers()
// Return:     std::map<uint, IMessageHandler_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> TestingMessageServiceFactory::CreateMessageHandlers(void)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map;

	msg_hdl_map[MSG_ID_ECHO] = std::make_shared<EchoMessageHandler>();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<TestingLoginHandler>();

	msg_hdl_map[MSG_ID_SUB_MARKETDATA] = std::make_shared<TestingSubMarketDataHandler>();

	msg_hdl_map[MSG_ID_UNSUB_MARKETDATA] = std::make_shared<TestingUnsubMarketDataHandler>();

	msg_hdl_map[MSG_ID_RET_MARKETDATA] = std::make_shared<TestingReturnMarketDataHandler>();

	msg_hdl_map[MSG_ID_ORDER_NEW] = std::make_shared<TestingCreateOrderHandler>();

	msg_hdl_map[MSG_ID_ORDER_CANCEL] = std::make_shared<TestingCancelOrderHandler>();

	msg_hdl_map[MSG_ID_QUERY_POSITION] = std::make_shared<TestingPositionHandler>();

	msg_hdl_map[MSG_ID_QUERY_ACCOUNT_INFO] = std::make_shared<TestingAccountInfoHandler>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       TestingMessageServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of TestingMessageServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> TestingMessageServiceFactory::CreateDataSerializers(void)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       TestingMessageServiceFactory::CreateWorkProcessor()
// Purpose:    Implementation of TestingMessageServiceFactory::CreateWorkProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<std::string, IProcessorBase_Ptr> TestingMessageServiceFactory::CreateWorkProcessor(void)
{
	return std::map<std::string, IProcessorBase_Ptr>();
}

////////////////////////////////////////////////////////////////////////
// Name:       TestingMessageServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of TestingMessageServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr TestingMessageServiceFactory::CreateMessageProcessor(void)
{
	return std::make_shared<TestingServerMessageProcessor>(_configMap);
}