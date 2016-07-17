/***********************************************************************
 * Module:  MessageProcessor.cpp
 * Author:  milk
 * Modified: 2014年10月6日 13:51:19
 * Purpose: Implementation of the class MessageProcessor
 ***********************************************************************/

#include "MessageProcessor.h"
#include "ContextAttribute.h"
#include "../litelogger/LiteLogger.h"

MessageProcessor::MessageProcessor()
{
}

MessageProcessor::~MessageProcessor()
{
	LOG_DEBUG << __FUNCTION__;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageProcessor::getSession()
// Purpose:    Implementation of MessageProcessor::getSession()
// Return:     session_ptr
////////////////////////////////////////////////////////////////////////

IMessageSession_Ptr MessageProcessor::getSession(void)
{
	return _msgsession_wk_ptr.lock();
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageProcessor::setSession(IMessageSession* msgSession)
// Purpose:    Implementation of MessageProcessor::setSession()
// Parameters:
// - msgSession
// Return:     void
////////////////////////////////////////////////////////////////////////

void MessageProcessor::setSession(IMessageSession_WkPtr msgSession_wk_ptr)
{
	_msgsession_wk_ptr = msgSession_wk_ptr;
}

void MessageProcessor::setServiceLocator(IMessageServiceLocator_Ptr svcLct_Ptr)
{
	_svc_locator_ptr = svcLct_Ptr;
}

void MessageProcessor::setServerContext(IContextAttribute * serverCtx)
{
	_serverCtx = serverCtx;
}

IContextAttribute * MessageProcessor::getServerContext(void)
{
	return _serverCtx;
}

bool MessageProcessor::OnSessionClosing(void)
{
	return true;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageProcessor::getServiceLocator()
// Purpose:    Implementation of MessageProcessor::getServiceLocator()
// Return:     IMessageServiceLocator*
////////////////////////////////////////////////////////////////////////

IMessageServiceLocator_Ptr MessageProcessor::getServiceLocator(void)
{
	return _svc_locator_ptr;
}