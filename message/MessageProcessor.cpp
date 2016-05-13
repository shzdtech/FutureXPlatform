/***********************************************************************
 * Module:  MessageProcessor.cpp
 * Author:  milk
 * Modified: 2014年10月6日 13:51:19
 * Purpose: Implementation of the class MessageProcessor
 ***********************************************************************/

#include "MessageProcessor.h"
#include "ContextAttribute.h"
#include <glog/logging.h>

MessageProcessor::MessageProcessor()
{
	_pMsgSession = nullptr;
}

MessageProcessor::~MessageProcessor()
{
	DLOG(INFO) << __FUNCTION__;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageProcessor::getSession()
// Purpose:    Implementation of MessageProcessor::getSession()
// Return:     session_ptr
////////////////////////////////////////////////////////////////////////

IMessageSession* MessageProcessor::getSession(void)
{
	return _pMsgSession;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageProcessor::setSession(IMessageSession* msgSession)
// Purpose:    Implementation of MessageProcessor::setSession()
// Parameters:
// - msgSession
// Return:     void
////////////////////////////////////////////////////////////////////////

void MessageProcessor::setSession(IMessageSession* pMsgSession)
{
	_pMsgSession = pMsgSession;
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

////////////////////////////////////////////////////////////////////////
// Name:       MessageProcessor::getServiceLocator()
// Purpose:    Implementation of MessageProcessor::getServiceLocator()
// Return:     IMessageServiceLocator*
////////////////////////////////////////////////////////////////////////

IMessageServiceLocator_Ptr MessageProcessor::getServiceLocator(void)
{
	return _svc_locator_ptr;
}