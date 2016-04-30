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
// Name:       TestingLoginHandler::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of TestingLoginHandler::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr TestingLoginHandler::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();
	auto& data = stdo->Data;

	auto& brokeid = TUtil::FirstNamedEntry(STR_BROKER_ID, data, EMPTY_STRING);
	auto& userid = TUtil::FirstNamedEntry(STR_USER_ID, data, EMPTY_STRING);
	auto& password = TUtil::FirstNamedEntry(STR_PASSWORD, data, EMPTY_STRING);
	
	auto pUserInfo = session->getUserInfo();
	pUserInfo->setInvestorId(userid);
	pUserInfo->setBrokerId(brokeid);
	pUserInfo->setCompany(userid);
	pUserInfo->setContactNum("110");
	pUserInfo->setEmail("cookie@gmail.com");
	pUserInfo->setName(userid);
	pUserInfo->setPassword(password);
	pUserInfo->setUserId(userid);
	pUserInfo->setRole(ROLE_CLIENT);
	pUserInfo->setPermission(ALLOW_TRADING);

	session->setLoginStatus(true);

	//
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