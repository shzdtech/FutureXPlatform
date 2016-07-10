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

const data_buffer ZERO_RETURN(new byte[1]{0}, 1);

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

void ServerSessionManager::AssembleSession(IMessageSession_Ptr msgSessionPtr)
{
	auto msgProcessor = _server->GetServiceFactory()->CreateMessageProcessor();
	msgProcessor->setServerContext(_server->getContext());
	msgProcessor->setServiceLocator(_msgsvclocator);
	msgSessionPtr->RegistProcessor(msgProcessor);
	if (msgSessionPtr->Start())
	{
		_sessionSet.insert(msgSessionPtr);
		msgSessionPtr->WriteMessage(MSG_ID_SESSION_CREATED, ZERO_RETURN);
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       ServerSessionManager::OnSessionClosing(IMessageSession_Ptr msgSessionPtr)
// Purpose:    Implementation of ServerSessionManager::OnSessionClosing()
// Parameters:
// - msgSessionPtr
// Return:     void
////////////////////////////////////////////////////////////////////////

void ServerSessionManager::OnSessionClosing(IMessageSession_Ptr msgSessionPtr)
{
	if (_sessionSet.find(msgSessionPtr) != _sessionSet.end())
		_sessionSet.erase(msgSessionPtr);
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
	auto it = _sessionSet.begin();
	while (it != _sessionSet.end())
	{
		try
		{
			auto sessionPtr = *it;
			_sessionSet.erase(it);
			if (sessionPtr)
				sessionPtr->Close();
		}
		catch (...) {}

		it = _sessionSet.begin();
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       ServerSessionManager::OnServerStarting()
// Purpose:    Implementation of ServerSessionManager::OnServerStarting()
// Return:     void
////////////////////////////////////////////////////////////////////////

void ServerSessionManager::OnServerStarting(void)
{
	if (!_msgsvclocator)
		_msgsvclocator = std::make_shared<MessageServiceLocator>(_server->GetServiceFactory());
	
	auto& wkprocMap = _msgsvclocator->AllWorkProcessor();
	for (auto& it : wkprocMap)
	{
		auto msgSession_Ptr = std::make_shared<MessageSession>();
		auto msgProc_Ptr = std::static_pointer_cast<IMessageProcessor>(it.second);
		msgSession_Ptr->RegistProcessor(msgProc_Ptr);
		msgProc_Ptr->setServiceLocator(_msgsvclocator);
		msgProc_Ptr->setSession(msgSession_Ptr.get());
		_sessionSet.insert(msgSession_Ptr);
	}
}