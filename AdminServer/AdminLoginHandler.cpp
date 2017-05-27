/***********************************************************************
 * Module:  AdminLoginHandler.cpp
 * Author:  milk
 * Modified: 2016年5月14日 22:10:51
 * Purpose: Implementation of the class AdminLoginHandler
 ***********************************************************************/

#include "AdminLoginHandler.h"
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"

#include "../dataobject/UserInfoDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"

#include "../systemsettings/AppContext.h"
#include "../message/BizError.h"
#include "../message/UserInfo.h"

#include "../common/BizErrorIDs.h"

#include "../databaseop/UserInfoDAO.h"

#include "../common/Attribute_Key.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       AdminLoginHandler::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of AdminLoginHandler::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr AdminLoginHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	dataobj_ptr ret;

	auto& userInfo = session->getUserInfo();

	if (!session->getLoginTimeStamp())
	{
		auto stdo = (StringMapDO<std::string>*)reqDO.get();

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

		userInfo.setLoginTime(now);
		userInfo.setAuthorizedKey(std::to_string(std::rand()));

		userInfo.setBrokerId(userInfo_Ptr->Company);
		userInfo.setName(userInfo_Ptr->FirstName);
		userInfo.setPassword(userInfo_Ptr->Password);
		userInfo.setUserId(userInfo_Ptr->UserId);
		userInfo.setRole(userInfo_Ptr->Role);
		userInfo.setPermission(userInfo_Ptr->Permission);

		userInfo.setExtInfo(userInfo_Ptr);

		session->setLoginTimeStamp();

		auto userInfoCache = std::static_pointer_cast<IUserInfoPtrMap>(AppContext::GetData(STR_KEY_APP_USER_DETAIL));
		if (!userInfoCache)
		{
			userInfoCache = std::make_shared<IUserInfoPtrMap>();
			AppContext::SetData(STR_KEY_APP_USER_DETAIL, userInfoCache);
		}

		userInfoCache->emplace(userInfo.getUserId(), IUserInfo_Ptr(new UserInfo(userInfo)));
	}

	ret = std::static_pointer_cast<UserInfoDO>(userInfo.getExtInfo());

	return ret;
}