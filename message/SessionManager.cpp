/***********************************************************************
 * Module:  SessionManager.cpp
 * Author:  milk
 * Modified: 2014年10月3日 23:10:38
 * Purpose: Implementation of the class SessionManager
 ***********************************************************************/

#include "SessionManager.h"
#include "../litelogger/LiteLogger.h"
#ifdef _WIN32
#include <Windows.h>
#endif


 ////////////////////////////////////////////////////////////////////////
 // Name:       SessionManager::SessionManager(messagerouter_ptr msgrouter, msgsvc_factory_ptr msgsvcfactory)
 // Purpose:    Implementation of SessionManager::SessionManager()
 // Parameters:
 // - msgrouter
 // - msgsvcfactory
 // Return:     
 ////////////////////////////////////////////////////////////////////////

SessionManager::SessionManager(IMessageServer* server)
	: _sessionSet(2048), _server(server)
{
}

////////////////////////////////////////////////////////////////////////
// Name:       SessionManager::~SessionManager()
// Purpose:    Implementation of SessionManager::~SessionManager()
// Return:     
////////////////////////////////////////////////////////////////////////

SessionManager::~SessionManager()
{
}

////////////////////////////////////////////////////////////////////////
// Name:       SessionManager::OnServerStarting()
// Purpose:    Implementation of SessionManager::OnServerStarting()
// Return:     void
////////////////////////////////////////////////////////////////////////

void SessionManager::OnServerStarting(void)
{
	// TODO : implement
}

////////////////////////////////////////////////////////////////////////
// Name:       SessionManager::OnServerClosing(void)
// Purpose:    Implementation of SessionManager::OnServerClosing()
// Parameters:
// - params
// Return:     void
////////////////////////////////////////////////////////////////////////

void SessionManager::OnServerClosing(void)
{
	for (auto pair : _sessionSet.lock_table())
	{
		if(auto msgSession = pair.first->getMessageSession())
			msgSession->NotifyClosing();
	}
	_sessionSet.clear();

	if (_workerSession)
	{
		_workerSession->NotifyClosing();
	}

	_server->getContext()->Reset();
}

bool SessionManager::CloseSession(const IMessageSession_Ptr & sessionPtr)
{
	try
	{
		_sessionSet.erase(sessionPtr->LockMessageProcessor());
	}
	catch (std::exception& ex)
	{
		LOG_ERROR << ex.what();
	}
	catch (...)
	{
		LOG_ERROR << __FUNCTION__ << ": Access violation!";
		return false;
	}
}

void SessionManager::AddSession(const IMessageSession_Ptr& sessionPtr)
{
	if (auto processor_ptr = sessionPtr->LockMessageProcessor())
	{
		_sessionSet.insert(processor_ptr, true);
	}
}