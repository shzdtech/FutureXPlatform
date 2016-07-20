/***********************************************************************
 * Module:  RegisterUserHandler.cpp
 * Author:  milk
 * Modified: 2016年5月14日 22:58:35
 * Purpose: Implementation of the class RegisterUserHandler
 ***********************************************************************/

#include "RegisterUserHandler.h"
#include "../message/MessageUtility.h"
#include "../message/SysParam.h"

#include "../common/Attribute_Key.h"
#include "../dataobject/UserInfoDO.h"
#include "../databaseop/UserInfoDAO.h"
#include "../dataobject/ResultDO.h"

#include "../common/BizErrorIDs.h"

////////////////////////////////////////////////////////////////////////
// Name:       RegisterUserHandler::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of RegisterUserHandler::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr RegisterUserHandler::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	if (session->getUserInfo()->getRole() < ROLE_ADMIN)
		throw ApiException(APIErrorID::NO_PERMISSION, "No permission to create a new user.");

	if (auto* pUserInfoDO = (UserInfoDO*)reqDO.get())
	{
		if (UserInfoDAO::FindUser(pUserInfoDO->UserId))
			throw UserException(UserErrorID::USERID_HAS_EXISTED, '"' + pUserInfoDO->UserId + "\" has been registered.");

		if (pUserInfoDO->Company.length() < 1)
			pUserInfoDO->Company = SysParam::Get(STR_KEY_DEFAULT_CLIENT_SYMBOL);

		if (UserInfoDAO::InsertUser(*pUserInfoDO) != 0)
			throw DatabaseException("Fail to insert user info.");
	}

	return dataobj_ptr(new ResultDO(reqDO->SerialId));
}