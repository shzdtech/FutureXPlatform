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

#include "../message/EchoMsgHandler.h"
#include "../message/EchoMsgSerializer.h"
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

	msg_hdl_map[MSG_ID_ECHO] = std::make_shared<EchoMsgHandler>();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<TestingLoginHandler>();

	msg_hdl_map[MSG_ID_SUB_MARKETDATA] = std::make_shared<TestingSubMarketDataHandler>();

	msg_hdl_map[MSG_ID_UNSUB_MARKETDATA] = std::make_shared<TestingUnsubMarketDataHandler>();

	msg_hdl_map[MSG_ID_RET_MARKETDATA] = std::make_shared<TestingReturnMarketDataHandler>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       TestingMessageServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of TestingMessageServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> TestingMessageServiceFactory::CreateDataSerializers(void)
{
	return AbstractDataSerializerFactory::Instance()->CreateDataSerializers();
}

////////////////////////////////////////////////////////////////////////
// Name:       TestingMessageServiceFactory::CreateWorkProcessor()
// Purpose:    Implementation of TestingMessageServiceFactory::CreateWorkProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IProcessorBase_Ptr> TestingMessageServiceFactory::CreateWorkProcessor(void)
{
	return std::map<uint, IProcessorBase_Ptr>();
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