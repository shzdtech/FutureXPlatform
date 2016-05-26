/***********************************************************************
 * Module:  CTPOTCLogin.cpp
 * Author:  milk
 * Modified: 2015年8月1日 21:18:52
 * Purpose: Implementation of the class CTPOTCLogin
 ***********************************************************************/

#include "CTPOTCLogin.h"
#include "CTPOTCUserContextBuilder.h"

#include "../utility/Encoding.h"
#include "../utility/TUtil.h"

#include "../dataobject/UserInfoDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"

#include "../message/AppContext.h"
#include "../message/BizError.h"
#include "../message/UserInfo.h"
#include "../message/SysParam.h"

#include "../common/BizErrorIDs.h"

#include "../databaseop/UserInfoDAO.h"

#include "../common/Attribute_Key.h"

#include <glog/logging.h>


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCLogin::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, IMessageProcessor* session)
 // Purpose:    Implementation of CTPOTCLogin::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCLogin::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (MapDO<std::string>*)reqDO.get();
	if (!session->IsLogin())
	{
		auto& userid = stdo->TryFind(STR_USER_ID, EMPTY_STRING);
		auto& password = stdo->TryFind(STR_PASSWORD, EMPTY_STRING);

		bool userInCache = false;
		std::shared_ptr<UserInfoDO> userInfo_Ptr;

		if (auto userInfoCache = std::static_pointer_cast<IUserInfoPtrMap>
			(AppContext::GetData(STR_KEY_DEFAULT_CLIENT_SYMBOL)))
		{
			auto it = userInfoCache->find(userid);
			if (it != userInfoCache->end())
			{
				userInfo_Ptr = std::static_pointer_cast<UserInfoDO>
					(it->second->getAttribute(STR_KEY_USER_INFO_DETAIL));
				userInCache = true;
			}
		}
		
		// Try to load from database
		if(!userInCache) 
			userInfo_Ptr = UserInfoDAO::FindUser(userid);

		if (!userInfo_Ptr)
		{
			throw BizError(INVAID_USERNAME, "Invalid Username.");
		}
		else if (userInfo_Ptr->Password != password)
		{
			throw BizError(WRONG_PASSWORD, "Wrong Password.");
		}

		auto pUserInfo = session->getUserInfo();
		pUserInfo->setBrokerId(userInfo_Ptr->Company);
		pUserInfo->setName(userInfo_Ptr->FirstName);
		pUserInfo->setPassword(userInfo_Ptr->Password);
		pUserInfo->setUserId(userInfo_Ptr->UserId);
		pUserInfo->setRole(userInfo_Ptr->Role);
		pUserInfo->setPermission(userInfo_Ptr->Permission);

		CTPOTCUserContextBuilder::Instance()->BuildContext(session);

		session->getUserInfo()->setAttribute(STR_KEY_USER_INFO_DETAIL, userInfo_Ptr);

		session->setLoginStatus(true);
	}

	auto userInfoDO_Ptr = std::static_pointer_cast<UserInfoDO>
		(session->getUserInfo()->getAttribute(STR_KEY_USER_INFO_DETAIL));
	userInfoDO_Ptr->SerialId = stdo->SerialId;

	return userInfoDO_Ptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCLogin::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, IMessageProcessor* session)
// Purpose:    Implementation of CTPOTCLogin::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCLogin::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}

