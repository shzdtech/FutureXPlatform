/***********************************************************************
 * Module:  CTPOTCLogin.cpp
 * Author:  milk
 * Modified: 2015年8月1日 21:18:52
 * Purpose: Implementation of the class CTPOTCLogin
 ***********************************************************************/

#include "CTPOTCLogin.h"
#include "CTPOTCUserContextBuilder.h"

#include "../CTPServer/CTPRawAPI.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"

#include "../dataobject/UserInfoDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"

#include "../message/BizError.h"
#include "../message/UserInfo.h"
#include "../message/SysParam.h"

#include "../databaseop/UserInfoDAO.h"

#include <glog/logging.h>
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/CTPConstant.h"


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
		auto userInfo_Ptr = UserInfoDAO::FindUser(userid, password);

		if (!userInfo_Ptr)
		{
			throw BizError(INVAID_USERNAME_OR_PASSWORD, "Invalid Username or Passowrd.");
		}

		auto pUserInfo = session->getUserInfo();
		pUserInfo->setBrokerId(userInfo_Ptr->BrokerId);
		pUserInfo->setCompany(userInfo_Ptr->Company);
		pUserInfo->setContactNum(userInfo_Ptr->ContactNum);
		pUserInfo->setEmail(userInfo_Ptr->Email);
		pUserInfo->setName(userInfo_Ptr->Name);
		pUserInfo->setPassword(userInfo_Ptr->Password);
		pUserInfo->setUserId(userInfo_Ptr->UserId);
		pUserInfo->setRole(userInfo_Ptr->Role);
		pUserInfo->setPermission(userInfo_Ptr->Permission);

		CTPOTCUserContextBuilder::Instance()->BuildContext(session);

		session->setLoginStatus(true);
	}

	ret = GenUserInfoDO(session->getUserInfo().get());
	
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCLogin::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, IMessageProcessor* session)
// Purpose:    Implementation of CTPOTCLogin::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCLogin::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}

std::shared_ptr<UserInfoDO> CTPOTCLogin::GenUserInfoDO(IUserInfo* pUserInfo)
{
	auto pDO = std::make_shared<UserInfoDO>();

	pDO->BrokerId = pUserInfo->getBrokerId();
	pDO->Company = pUserInfo->getCompany();
	pDO->ContactNum = pUserInfo->getContactNum();
	pDO->Email = pUserInfo->getEmail();
	pDO->Name = pUserInfo->getName();
	//pDO->Password = pUserInfo->getPassword();
	pDO->Permission = pUserInfo->getPermission();
	pDO->Role = pUserInfo->getRole();
	pDO->UserId = pUserInfo->getUserId();

	return pDO;
}

