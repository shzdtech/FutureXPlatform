/***********************************************************************
 * Module:  CTSLoginHandler.cpp
 * Author:  milk
 * Modified: 2015年12月9日 15:51:46
 * Purpose: Implementation of the class CTSLoginHandler
 ***********************************************************************/

#include "CTSLoginHandler.h"
#include "CTSAPIWrapper.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../dataobject/UserInfoDO.h"

#include "../message/BizError.h"
#include "../message/IUserInfo.h"

#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include <glog/logging.h>

////////////////////////////////////////////////////////////////////////
// Name:       CTSLoginHandler::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSLoginHandler::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSLoginHandler::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();
	auto& data = stdo->Data;

	auto& brokeid = TUtil::FirstNamedEntry(STR_BROKER_ID, data, EMPTY_STRING);
	auto& userid = TUtil::FirstNamedEntry(STR_USER_ID, data, EMPTY_STRING);
	auto& password = TUtil::FirstNamedEntry(STR_PASSWORD, data, EMPTY_STRING);

	DLOG(INFO) << "Start login: " << brokeid << ":" << userid << ":" << password << std::endl;

	CTSAPIWrapper* api = (CTSAPIWrapper*)rawAPI;
	api->Impl()->Login(brokeid.data(), userid.data(), password.data());

	auto pUserInfo = session->getUserInfo();
	pUserInfo->setInvestorId(userid);
	pUserInfo->setBrokerId(brokeid);
	pUserInfo->setName(userid);
	pUserInfo->setPassword(password);
	pUserInfo->setUserId(userid);
	pUserInfo->setRole(ROLE_CLIENT);
	pUserInfo->setPermission(ALLOW_TRADING);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSLoginHandler::HandleResponse(param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSLoginHandler::HandleResponse()
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSLoginHandler::HandleResponse(param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
{
	auto errCode = *(int*)rawParams[0];
	if (errCode)
	{
		throw BizError(errCode, "");
	}

	auto pDO = new UserInfoDO;
	dataobj_ptr ret(pDO);

	auto pUserInfo = session->getUserInfo();

	pDO->UserId = pUserInfo->getUserId();
	pDO->BrokerId = pUserInfo->getBrokerId();
	//pDO->Password = pUserInfo->getPassword();
	pDO->FirstName = pUserInfo->getName();
	pDO->Role = pUserInfo->getRole();
	pDO->Permission = pUserInfo->getPermission();

	session->setLoginStatus(true);

	DLOG(INFO) << "Login successful." << std::endl;

	return ret;
}