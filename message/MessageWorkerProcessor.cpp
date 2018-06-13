#include "MessageWorkerProcessor.h"

MessageWorkerProcessor::MessageWorkerProcessor()
	: _isLogged(false), _isConnected(false),
	_userSessionCtn_Ptr(SessionContainer<std::string>::NewInstancePtr())
{
}

MessageWorkerProcessor::~MessageWorkerProcessor()
{
	_closing = true;
}

bool MessageWorkerProcessor::HasLogged(void)
{
	return _isLogged;
}

bool MessageWorkerProcessor::ConnectedToServer(void)
{
	return _isConnected;
}

const IUserInfo & MessageWorkerProcessor::GetSystemUser()
{
	return _systemUser;
}

void MessageWorkerProcessor::RegisterSession(const std::string& id, const IMessageSession_Ptr& sessionPtr)
{
	_userSessionCtn_Ptr->add(id, sessionPtr);
}

void MessageWorkerProcessor::RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr)
{
	auto& userInfo = sessionPtr->getUserInfo();
	RegisterSession(userInfo.getUserId(), sessionPtr);
}