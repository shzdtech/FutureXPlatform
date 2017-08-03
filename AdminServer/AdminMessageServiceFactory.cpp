/***********************************************************************
 * Module:  AdminMessageServiceFactory.cpp
 * Author:  milk
 * Modified: 2016年5月14日 22:09:43
 * Purpose: Implementation of the class AdminMessageServiceFactory
 ***********************************************************************/

#include "AdminMessageServiceFactory.h"
#include "AdminServerMessageProcessor.h"

#include "../message/EchoMessageHandler.h"
#include "../message/EchoMessageSerializer.h"
#include "../message/DefMessageID.h"
#include "../dataserializer/AbstractDataSerializerFactory.h"

#include "AdminLoginHandler.h"
#include "RegisterUserHandler.h"
#include "ChangePasswordHandler.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       AdminMessageServiceFactory::CreateMessageHandlers()
 // Purpose:    Implementation of AdminMessageServiceFactory::CreateMessageHandlers()
 // Return:     std::map<uint, IMessageHandler_Ptr>
 ////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> AdminMessageServiceFactory::CreateMessageHandlers(IServerContext* serverCtx)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map;

	msg_hdl_map[MSG_ID_ECHO] = std::make_shared<EchoMessageHandler>();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<AdminLoginHandler>();

	msg_hdl_map[MSG_ID_USER_NEW] = std::make_shared<RegisterUserHandler>();

	msg_hdl_map[MSG_ID_USER_INFO] = msg_hdl_map[MSG_ID_LOGIN];

	msg_hdl_map[MSG_ID_RESET_PASSWORD] = std::make_shared<ChangePasswordHandler>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       AdminMessageServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of AdminMessageServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> AdminMessageServiceFactory::CreateDataSerializers(IServerContext* serverCtx)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       AdminMessageServiceFactory::CreateWorkerProcessor()
// Purpose:    Implementation of AdminMessageServiceFactory::CreateWorkerProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr AdminMessageServiceFactory::CreateWorkerProcessor(IServerContext* serverCtx)
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       AdminMessageServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of AdminMessageServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr AdminMessageServiceFactory::CreateMessageProcessor(IServerContext* serverCtx)
{
	return std::make_shared<AdminServerMessageProcessor>();
}