/***********************************************************************
 * Module:  ServerSessionManager.cpp
 * Author:  milk
 * Modified: 2015年10月22日 21:48:47
 * Purpose: Implementation of the class ServerSessionManager
 ***********************************************************************/

#include "ServerSessionManager.h"
#include "MessageServiceLocator.h"
#include "MessageSession.h"
#include "DefMessageID.h"
#include "../dataobject/ResultDO.h"

static const data_buffer ZERO_RETURN(new byte[1]{ 0 }, 1);

////////////////////////////////////////////////////////////////////////
// Name:       ServerSessionManager::ServerSessionManager()
// Purpose:    Implementation of ServerSessionManager::ServerSessionManager()
// Return:     
////////////////////////////////////////////////////////////////////////

ServerSessionManager::ServerSessionManager(IMessageServer* server)
	: SessionManager(server)
{
}

////////////////////////////////////////////////////////////////////////
// Name:       ServerSessionManager::~ServerSessionManager()
// Purpose:    Implementation of ServerSessionManager::~ServerSessionManager()
// Return:     
////////////////////////////////////////////////////////////////////////

ServerSessionManager::~ServerSessionManager()
{
	// TODO : implement
}

////////////////////////////////////////////////////////////////////////
// Name:       ServerSessionManager::AssembleSession(IMessageSession_Ptr msgSessionPtr)
// Purpose:    Implementation of ServerSessionManager::AssembleSession()
// Parameters:
// - msgSessionPtr
// Return:     void
////////////////////////////////////////////////////////////////////////

void ServerSessionManager::AssembleSession(const IMessageSession_Ptr& msgSessionPtr)
{
	auto msgProcessor = _server->getServiceFactory()->CreateMessageProcessor(_server->getContext());
	msgProcessor->setServerContext(_server->getContext());
	msgProcessor->setServiceLocator(_msgsvclocator);
	msgSessionPtr->RegisterProcessor(msgProcessor);
	AddSession(msgSessionPtr);

	if (msgSessionPtr->Start())
	{
		msgSessionPtr->WriteMessage(MSG_ID_SESSION_CREATED, ZERO_RETURN);
	}
	else
	{
		CloseSession(msgSessionPtr);
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       ServerSessionManager::OnServerClosing(void)
// Purpose:    Implementation of ServerSessionManager::OnServerClosing()
// Parameters:
// - params
// Return:     void
////////////////////////////////////////////////////////////////////////

//void ServerSessionManager::OnServerClosing(void)
//{
//	auto processors = _sessionSet.lock_table();
//	for (auto pair : processors)
//	{
//		try
//		{
//			pair.first->getMessageSession()->Close();
//		}
//		catch (...) {}
//	}
//	processors.release();
//	_sessionSet.clear();
//	_server->getContext()->CancelUserOrders();
//}

////////////////////////////////////////////////////////////////////////
// Name:       ServerSessionManager::OnServerStarting()
// Purpose:    Implementation of ServerSessionManager::OnServerStarting()
// Return:     void
////////////////////////////////////////////////////////////////////////

void ServerSessionManager::OnServerStarting(void)
{
	_msgsvclocator = std::make_shared<MessageServiceLocator>(_server->getServiceFactory(), _server->getContext());

	if (auto workProcPtr = _server->getContext()->getWorkerProcessor())
	{
		auto workerSession = std::make_shared<MessageSession>(shared_from_this());
		workProcPtr->setMessageSession(workerSession);
		workProcPtr->setServiceLocator(_msgsvclocator);
		workerSession->RegisterProcessor(workProcPtr);
	}
}