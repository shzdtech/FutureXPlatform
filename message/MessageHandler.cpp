#include "MessageHandler.h"
#include "BizError.h"
#include "../common/BizErrorIDs.h"

std::atomic_uint MessageHandler::_requestIdGen;

bool MessageHandler::CheckLogin(const IMessageSession_Ptr& session, bool throwBizErr)
{
	bool ret = session->getLoginTimeStamp();

	if (!ret && throwBizErr)
	{
		throw UserException(STATUS_NOT_LOGIN, "NOT LOGIN!");
	}

	return ret;
}

bool MessageHandler::CheckRolePermission(const IMessageSession_Ptr & session, UserRoleType leastRole, bool throwBizErr)
{
	bool ret = session->getUserInfo().getRole() >= leastRole;

	if (!ret && throwBizErr)
	{
		throw ApiException(NO_PERMISSION, "NO PERMISSION!");
	}

	return ret;
}
