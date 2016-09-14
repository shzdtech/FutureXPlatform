/***********************************************************************
 * Module:  AdminLoginHandler.cpp
 * Author:  milk
 * Modified: 2016年5月14日 22:10:51
 * Purpose: Implementation of the class AdminLoginHandler
 ***********************************************************************/

#include "AdminLoginHandler.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"

#include "../dataobject/UserInfoDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"

#include "../message/AppContext.h"
#include "../message/BizError.h"
#include "../message/UserInfo.h"

#include "../common/BizErrorIDs.h"

#include "../databaseop/UserInfoDAO.h"

#include "../common/Attribute_Key.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       AdminLoginHandler::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of AdminLoginHandler::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr AdminLoginHandler::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	dataobj_ptr ret;

	auto pUserInfo = session->getUserInfo();

	if (!session->getLoginTimeStamp())
	{
		auto stdo = (MapDO<std::string>*)reqDO.get();

		auto& userid = stdo->TryFind(STR_USER_NAME, EMPTY_STRING);
		auto& password = stdo->TryFind(STR_PASSWORD, EMPTY_STRING);
		auto userInfo_Ptr = UserInfoDAO::FindUser(userid);

		if (!userInfo_Ptr)
		{
			throw UserException(USERID_NOT_EXITS, "UserId: " + userid + " not exists.");
		}
		else if (userInfo_Ptr->Password != password)
		{
			throw UserException(WRONG_PASSWORD, "Wrong password.");
		}

		auto now = std::time(nullptr);

		pUserInfo->setLoginTime(now);
		pUserInfo->setAuthorizedKey(std::to_string(std::rand()));

		pUserInfo->setBrokerId(userInfo_Ptr->Company);
		pUserInfo->setName(userInfo_Ptr->FirstName);
		pUserInfo->setPassword(userInfo_Ptr->Password);
		pUserInfo->setUserId(userInfo_Ptr->UserId);
		pUserInfo->setRole(userInfo_Ptr->Role);
		pUserInfo->setPermission(userInfo_Ptr->Permission);

		pUserInfo->setExtInfo(userInfo_Ptr);

		session->setLoginTimeStamp();

		auto userInfoCache = std::static_pointer_cast<IUserInfoPtrMap>(AppContext::GetData(STR_KEY_APP_USER_DETAIL));
		if (!userInfoCache)
		{
			userInfoCache = std::make_shared<IUserInfoPtrMap>();
			AppContext::SetData(STR_KEY_APP_USER_DETAIL, userInfoCache);
		}

		userInfoCache->emplace(pUserInfo->getUserId(), pUserInfo);
	}

	ret = std::static_pointer_cast<UserInfoDO>(pUserInfo->getExtInfo());

	return ret;
}