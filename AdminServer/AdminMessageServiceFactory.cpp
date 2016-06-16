/***********************************************************************
 * Module:  AdminMessageServiceFactory.cpp
 * Author:  milk
 * Modified: 2016年5月14日 22:09:43
 * Purpose: Implementation of the class AdminMessageServiceFactory
 ***********************************************************************/

#include "AdminMessageServiceFactory.h"
#include "AdminServerMessageProcessor.h"

#include "../message/EchoMsgHandler.h"
#include "../message/EchoMsgSerializer.h"
#include "../message/DefMessageID.h"
#include "../dataobject/AbstractDataSerializerFactory.h"

#include "AdminLoginHandler.h"
#include "RegisterUserHandler.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       AdminMessageServiceFactory::CreateMessageHandlers()
 // Purpose:    Implementation of AdminMessageServiceFactory::CreateMessageHandlers()
 // Return:     std::map<uint, IMessageHandler_Ptr>
 ////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> AdminMessageServiceFactory::CreateMessageHandlers(void)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map;

	msg_hdl_map[MSG_ID_ECHO] = std::make_shared<EchoMsgHandler>();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<AdminLoginHandler>();

	msg_hdl_map[MSG_ID_USER_NEW] = std::make_shared<RegisterUserHandler>();

	msg_hdl_map[MSG_ID_USER_INFO] = msg_hdl_map[MSG_ID_LOGIN];

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       AdminMessageServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of AdminMessageServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> AdminMessageServiceFactory::CreateDataSerializers(void)
{
	return AbstractDataSerializerFactory::Instance()->CreateDataSerializers();
}

////////////////////////////////////////////////////////////////////////
// Name:       AdminMessageServiceFactory::CreateWorkProcessor()
// Purpose:    Implementation of AdminMessageServiceFactory::CreateWorkProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<std::string, IProcessorBase_Ptr> AdminMessageServiceFactory::CreateWorkProcessor(void)
{
	return std::map<std::string, IProcessorBase_Ptr>();
}

////////////////////////////////////////////////////////////////////////
// Name:       AdminMessageServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of AdminMessageServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr AdminMessageServiceFactory::CreateMessageProcessor(void)
{
	return std::make_shared<AdminServerMessageProcessor>();
}

////////////////////////////////////////////////////////////////////////
// Name:       AdminMessageServiceFactory::Load(const std::string& configFile, const std::string& param)
// Purpose:    Implementation of AdminMessageServiceFactory::Load()
// Parameters:
// - configFile
// - param
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool AdminMessageServiceFactory::Load(const std::string& configFile, const std::string& param)
{
	return true;
}