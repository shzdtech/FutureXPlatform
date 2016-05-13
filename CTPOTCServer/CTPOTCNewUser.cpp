/***********************************************************************
 * Module:  CTPOTCNewUser.cpp
 * Author:  milk
 * Modified: 2016年5月12日 17:33:04
 * Purpose: Implementation of the class CTPOTCNewUser
 ***********************************************************************/

#include "CTPOTCNewUser.h"
#include "../CTPServer/CTPConstant.h"

#include "../message/GlobalProcessorRegistry.h"
#include "../message/SysParam.h"

#include "CTPWorkerProcessorID.h"
#include "../dataobject/UserInfoDO.h"
#include "../databaseop/UserInfoDAO.h"
#include "../dataobject/ResultDO.h"

#include "../common/BizErrorIDs.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCNewUser::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPOTCNewUser::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCNewUser::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	if (session->getUserInfo()->getRole() != ROLE_ADMINCLIENT)
		throw BizError(APIErrorID::NO_PERMISSION, "You do not have the permission to create a new user.");

	if (auto* pUserInfoDO = (UserInfoDO*)reqDO.get())
	{
		if (UserInfoDAO::FindUser(pUserInfoDO->UserId))
			throw BizError(UserErrorID::USERID_HAS_EXISTED, "This user name has been registered.");

		if (pUserInfoDO->Company.length() < 1)
			SysParam::TryGet(DEFAULT_CLIENT_SYMBOL, pUserInfoDO->Company);

		if (UserInfoDAO::InsertUser(*pUserInfoDO) != 0)
			throw BizError(ResultType::SYS_ERROR, "Fail to insert user info.");
	}

	return dataobj_ptr(new ResultDO());
}