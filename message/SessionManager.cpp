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
	: _processorSet(256), _server(server)
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
	for (auto pair : _processorSet.lock_table())
	{
		if (auto session = pair.first->getMessageSession())
		{
			session->NotifyClosing();
		}
		else
		{
			pair.first->OnSessionClosing();
		}
	}
	_processorSet.clear();

	if (auto workProcPtr = _server->getContext()->getWorkerProcessor())
	{
		if (auto session = workProcPtr->getMessageSession())
		{
			session->NotifyClosing();
		}
		else
		{
			workProcPtr->OnSessionClosing();
		}
	}

	_server->getContext()->Reset();
}

bool SessionManager::CloseSession(const IMessageSession_Ptr & sessionPtr)
{
	try
	{
		if (auto processor = sessionPtr->LockMessageProcessor())
			_processorSet.erase(processor);
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
	if (auto processor = sessionPtr->LockMessageProcessor())
		_processorSet.insert(processor, true);
}