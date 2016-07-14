/***********************************************************************
 * Module:  TestingLoginHandler.cpp
 * Author:  milk
 * Modified: 2016年4月29日 19:24:33
 * Purpose: Implementation of the class TestingLoginHandler
 ***********************************************************************/

#include "TestingLoginHandler.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../dataobject/UserInfoDO.h"

#include "../utility/Encoding.h"
#include "../utility/TUtil.h"

////////////////////////////////////////////////////////////////////////
// Name:       TestingLoginHandler::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of TestingLoginHandler::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr TestingLoginHandler::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& brokeid = stdo->TryFind(STR_BROKER_ID, EMPTY_STRING);
	auto& userid = stdo->TryFind(STR_USER_NAME, EMPTY_STRING);
	auto& password = stdo->TryFind(STR_PASSWORD, EMPTY_STRING);
	
	auto pUserInfo = session->getUserInfo();
	pUserInfo->setInvestorId(userid);
	pUserInfo->setBrokerId(brokeid);
	pUserInfo->setName(userid);
	pUserInfo->setPassword(password);
	pUserInfo->setUserId(userid);
	pUserInfo->setRole(ROLE_CLIENT);
	pUserInfo->setPermission(ALLOW_TRADING);

	session->setLoginStatus(true);

	//
	auto pDO = std::make_shared<UserInfoDO>();
	pDO->SerialId = reqDO->SerialId;

	pDO->BrokerId = pUserInfo->getBrokerId();;
	pDO->UserName = pUserInfo->getName();
	pDO->Password = pUserInfo->getPassword();
	pDO->Permission = pUserInfo->getPermission();
	pDO->Role = pUserInfo->getRole();
	pDO->UserId = pUserInfo->getUserId();

	return pDO;
}