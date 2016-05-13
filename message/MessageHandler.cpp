#include "MessageHandler.h"
#include "BizError.h"
#include "../common/BizErrorIDs.h"

std::atomic_uint MessageHandler::_requestIdGen;

bool MessageHandler::CheckLogin(ISession* session, bool throwBizErr)
{
	bool ret = session->IsLogin();

	if (!ret && throwBizErr)
	{
		throw BizError(STATUS_NOT_LOGIN, "NOT LOGIN!");
	}

	return ret;
}
