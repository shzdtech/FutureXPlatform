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
	dataobj_ptr ret;

	if (!session->IsLogin())
	{
		auto stdo = (StringTableDO*)reqDO.get();
		auto& data = stdo->Data;

		auto& userid = TUtil::FirstNamedEntry(STR_USER_ID, data, EMPTY_STRING);
		auto& password = TUtil::FirstNamedEntry(STR_PASSWORD, data, EMPTY_STRING);
		auto userInfo_Ptr = UserInfoDAO::FindUser(userid);

		if (!userInfo_Ptr)
		{
			throw BizError(INVAID_USERNAME, "Invalid Username.");
		}
		else if(userInfo_Ptr->Password != password)
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

	ret = std::static_pointer_cast<UserInfoDO>
		(session->getUserInfo()->getAttribute(STR_KEY_USER_INFO_DETAIL));
	
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCLogin::HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, IMessageProcessor* session)
// Purpose:    Implementation of CTPOTCLogin::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCLogin::HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}

