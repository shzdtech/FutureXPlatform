/***********************************************************************
 * Module:  MessageSession.cpp
 * Author:  milk
 * Modified: 2014年10月6日 9:55:49
 * Purpose: Implementation of the class MessageSession
 ***********************************************************************/

#include "MessageSession.h"
#include "MessageContext.h"
#include "UserInfo.h"
#include "../litelogger/LiteLogger.h"


////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::MessageSession()
// Purpose:    Implementation of MessageSession::MessageSession()
// Return:     
////////////////////////////////////////////////////////////////////////

MessageSession::MessageSession(const ISessionManager_Ptr& sessionMgr_Ptr)
	: _sessionManager_ptr(sessionMgr_Ptr), _userInfo_ptr(new UserInfo), _context_ptr(new MessageContext),
	_timeout(0), _loginTimeStamp(0)
{
	static uint64_t idgen = 0;
	_id = ++idgen;
}

uint64_t MessageSession::Id()
{
	return _id;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::~MessageSession()
// Purpose:    Implementation of MessageSession::~MessageSession()
// Return:     
////////////////////////////////////////////////////////////////////////

MessageSession::~MessageSession()
{
	/*if (!_closed)
	{
		Close();
	}*/
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::RegistProcessor(session_ptr session)
// Purpose:    Implementation of MessageSession::RegistProcessor()
// Parameters:
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

void MessageSession::RegistProcessor(const IMessageProcessor_Ptr& msgProcessor)
{
	_messageProcessor_ptr = msgProcessor;
	_messageProcessor_ptr->setSession(shared_from_this());
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::getContext()
// Purpose:    Implementation of MessageProcessor::getContext()
// Return:     context_ptr
////////////////////////////////////////////////////////////////////////

IMessageContext_Ptr& MessageSession::getContext(void)
{
	return _context_ptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::setTimeout(int seconds)
// Purpose:    Implementation of MessageSession::setTimeout()
// Parameters:
// - seconds
// Return:     void
////////////////////////////////////////////////////////////////////////

void MessageSession::setTimeout(long seconds)
{
	_timeout = seconds;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::Close()
// Purpose:    Implementation of MessageSession::Close()
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool MessageSession::Close(void)
{
	bool ret = true;
	if (_sessionManager_ptr)
	{
		try
		{
			ret = _sessionManager_ptr->CloseSession(shared_from_this());
		}
		catch (...) {}
	}

	return ret;
}

bool MessageSession::NotifyClosing(void)
{
	bool ret = false;
	if (_messageProcessor_ptr &&
		_messageProcessor_ptr->OnSessionClosing())
	{
		auto this_ptr = shared_from_this();
		IMessageSessionEvent_WkPtr event_wkptr;
		while (_sessionHub.pop(event_wkptr))
		{
			if (auto event_ptr = event_wkptr.lock())
			{
				event_ptr->OnSessionClosing(this_ptr);
			}
		}

		ret = true;
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::addListener(ISessionManager_Ptr manager)
// Purpose:    Implementation of MessageSession::addListener()
// Parameters:
// - manager
// Return:     void
////////////////////////////////////////////////////////////////////////

void MessageSession::addListener(const IMessageSessionEvent_WkPtr& listener)
{
	_sessionHub.emplace(listener);
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::removeListener(ISessionManager_Ptr manager)
// Purpose:    Implementation of MessageSession::removeListener()
// Parameters:
// - manager
// Return:     void
////////////////////////////////////////////////////////////////////////

void MessageSession::removeListener(const IMessageSessionEvent_WkPtr& listener)
{
	_sessionHub.erase(listener);
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::getLoginTimeStamp()
// Purpose:    Implementation of MessageSession::getLoginTimeStamp()
// Return:     bool
////////////////////////////////////////////////////////////////////////

time_t MessageSession::getLoginTimeStamp(void)
{
	return _loginTimeStamp;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::SetLoginStatus(bool status)
// Purpose:    Implementation of MessageSession::SetLoginStatus()
// Parameters:
// - status
// Return:     void
////////////////////////////////////////////////////////////////////////

void MessageSession::setLoginTimeStamp(time_t tm)
{
	if (!tm)
	{
		tm = time(nullptr);
	}

	_loginTimeStamp = tm;
}

void MessageSession::setLogout(void)
{
	_loginTimeStamp = 0;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::getUserInfo()
// Purpose:    Implementation of MessageSession::getUserInfo()
// Return:     userinfo_ptr
////////////////////////////////////////////////////////////////////////

IUserInfo_Ptr& MessageSession::getUserInfo(void)
{
	return _userInfo_ptr;
}


////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::getProcessor()
// Purpose:    Implementation of MessageSession::getProcessor()
// Return:     IProcessorBase*
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr& MessageSession::getProcessor(void)
{
	return _messageProcessor_ptr;
}


void MessageSession::setSessionManager(const ISessionManager_Ptr& sessionMgr_Ptr)
{
	_sessionManager_ptr = sessionMgr_Ptr;
}

ISessionManager_Ptr& MessageSession::getSessionManager(void)
{
	return _sessionManager_ptr;
}