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
