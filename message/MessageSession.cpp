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

#include <atomic>

uint64_t MessageSession::Id()
{
	return _id;
}


////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::MessageSession()
// Purpose:    Implementation of MessageSession::MessageSession()
// Return:     
////////////////////////////////////////////////////////////////////////

MessageSession::MessageSession(const ISessionManager_Ptr& sessionMgr_Ptr)
	: _sessionManager_ptr(sessionMgr_Ptr), _timeout(0), _loginTimeStamp(0), _sessionHub(2)
{
	static std::atomic_uint64_t idgen {};
	_id = ++idgen;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::~MessageSession()
// Purpose:    Implementation of MessageSession::~MessageSession()
// Return:     
////////////////////////////////////////////////////////////////////////
MessageSession::~MessageSession()
{
	LOG_DEBUG << __FUNCTION__;
}


////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::RegisterProcessor(session_ptr session)
// Purpose:    Implementation of MessageSession::RegisterProcessor()
// Parameters:
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

void MessageSession::RegisterProcessor(const IMessageProcessor_Ptr& msgprocessor_ptr)
{
	_messageProcessor_wkptr = msgprocessor_ptr;
	msgprocessor_ptr->setMessageSession(shared_from_this());
}


////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::getContext()
// Purpose:    Implementation of MessageProcessor::getContext()
// Return:     context_ptr
////////////////////////////////////////////////////////////////////////

IMessageContext_Ptr& MessageSession::getContext(void)
{
	if (!_context_ptr)
		_context_ptr.reset(new MessageContext);

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
			if (NotifyClosing())
			{
				ret = _sessionManager_ptr->CloseSession(shared_from_this());
			}
		}
		catch (std::exception& ex)
		{
			LOG_ERROR << __FUNCTION__ << ex.what();
		}
		catch (...)
		{
			LOG_ERROR << __FUNCTION__ << ": Unknown error";
		}
	}

	return ret;
}

bool MessageSession::NotifyClosing(void)
{
	bool ret = true;

	if (auto msgProcessor_Ptr = LockMessageProcessor())
	{
		ret = msgProcessor_Ptr->OnSessionClosing();
	}

	if (ret)
	{
		auto this_ptr = shared_from_this();
		for (auto pair : _sessionHub.lock_table())
		{
			if (auto event_ptr = pair.first.lock())
			{
				event_ptr->OnSessionClosing(this_ptr);
			}
		}
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
	_sessionHub.insert(listener, true);
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

IUserInfo& MessageSession::getUserInfo(void)
{
	return _userInfo;
}


////////////////////////////////////////////////////////////////////////
// Name:       MessageSession::LockMessageProcessor()
// Purpose:    Implementation of MessageSession::LockMessageProcessor()
// Return:     IProcessorBase*
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr MessageSession::LockMessageProcessor(void)
{
	return _messageProcessor_wkptr.lock();
}


void MessageSession::setSessionManager(const ISessionManager_Ptr& sessionMgr_Ptr)
{
	_sessionManager_ptr = sessionMgr_Ptr;
}

ISessionManager_Ptr& MessageSession::getSessionManager(void)
{
	return _sessionManager_ptr;
}