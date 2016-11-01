/***********************************************************************
 * Module:  SessionManager.cpp
 * Author:  milk
 * Modified: 2014年10月3日 23:10:38
 * Purpose: Implementation of the class SessionManager
 ***********************************************************************/

#include "SessionManager.h"

////////////////////////////////////////////////////////////////////////
// Name:       SessionManager::SessionManager(messagerouter_ptr msgrouter, msgsvc_factory_ptr msgsvcfactory)
// Purpose:    Implementation of SessionManager::SessionManager()
// Parameters:
// - msgrouter
// - msgsvcfactory
// Return:     
////////////////////////////////////////////////////////////////////////

SessionManager::SessionManager(IMessageServer* server)
{
	_server = server;
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
	_sessionSet.clear();
	_server->getContext()->Reset();
}

////////////////////////////////////////////////////////////////////////
// Name:       SessionManager::OnSessionClosing(session_ptr sessionPtr)
// Purpose:    Implementation of SessionManager::OnSessionClosing()
// Parameters:
// - sessionPtr
// Return:     bool
////////////////////////////////////////////////////////////////////////

void SessionManager::OnSessionClosing(const IMessageSession_Ptr& sessionPtr)
{
	_sessionSet.erase(sessionPtr);
}