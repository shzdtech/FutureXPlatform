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

const data_buffer ZERO_RETURN(new byte[1]{ 0 }, 1);

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
	auto msgProcessor = _server->GetServiceFactory()->CreateMessageProcessor(_server->getContext());
	msgProcessor->setServerContext(_server->getContext());
	msgProcessor->setServiceLocator(_msgsvclocator);
	msgSessionPtr->RegistProcessor(msgProcessor);
	if (msgSessionPtr->Start())
	{
		_sessionMap.insert(msgSessionPtr.get(), msgSessionPtr);
		msgSessionPtr->WriteMessage(MSG_ID_SESSION_CREATED, ZERO_RETURN);
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       ServerSessionManager::OnServerClosing(void)
// Purpose:    Implementation of ServerSessionManager::OnServerClosing()
// Parameters:
// - params
// Return:     void
////////////////////////////////////////////////////////////////////////

void ServerSessionManager::OnServerClosing(void)
{
	std::vector<IMessageSession_Ptr> sessionvector;
	{
		auto lt = _sessionMap.lock_table();
		for (auto& pair : lt)
		{
			sessionvector.push_back(pair.second);
		}
	}
	for (auto& sessionPtr : sessionvector)
	{
		try
		{
			sessionPtr->Close();
		}
		catch (...) {}
	}
	_server->getContext()->Reset();
}

////////////////////////////////////////////////////////////////////////
// Name:       ServerSessionManager::OnServerStarting()
// Purpose:    Implementation of ServerSessionManager::OnServerStarting()
// Return:     void
////////////////////////////////////////////////////////////////////////

void ServerSessionManager::OnServerStarting(void)
{
	_msgsvclocator = std::make_shared<MessageServiceLocator>(_server->GetServiceFactory(), _server->getContext());

	if (auto workProcPtr = _server->getContext()->getWorkerProcessor())
	{
		auto msgSession_Ptr = std::make_shared<MessageSession>();
		workProcPtr->setServiceLocator(_msgsvclocator);
		msgSession_Ptr->RegistProcessor(workProcPtr);
		workProcPtr->setSession(msgSession_Ptr);

		_sessionMap.insert(msgSession_Ptr.get(), msgSession_Ptr);
	}
}