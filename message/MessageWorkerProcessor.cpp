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

void MessageWorkerProcessor::RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr)
{
	auto& userInfo = sessionPtr->getUserInfo();
	//userInfo.setBrokerId(_systemUser.getBrokerId());
	//userInfo.setInvestorId(_systemUser.getInvestorId());
	//userInfo.setFrontId(_systemUser.getFrontId());
	//userInfo.setSessionId(_systemUser.getSessionId());
	_userSessionCtn_Ptr->add(userInfo.getUserId(), sessionPtr);
}
