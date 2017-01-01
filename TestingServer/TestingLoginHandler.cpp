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

#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"

////////////////////////////////////////////////////////////////////////
// Name:       TestingLoginHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of TestingLoginHandler::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr TestingLoginHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& brokeid = stdo->TryFind(STR_BROKER_ID, EMPTY_STRING);
	auto& userid = stdo->TryFind(STR_USER_NAME, EMPTY_STRING);
	auto& password = stdo->TryFind(STR_PASSWORD, EMPTY_STRING);
	
	auto& userInfo = session->getUserInfo();
	userInfo.setInvestorId(userid);
	userInfo.setBrokerId(brokeid);
	userInfo.setName(userid);
	userInfo.setPassword(password);
	userInfo.setUserId(userid);
	userInfo.setRole(ROLE_CLIENT);
	userInfo.setPermission(ALLOW_TRADING);

	session->setLoginTimeStamp();

	//
	auto pDO = std::make_shared<UserInfoDO>();

	pDO->BrokerId = userInfo.getBrokerId();
	pDO->Company = userInfo.getBrokerId();
	pDO->UserName = userInfo.getName();
	pDO->Password = userInfo.getPassword();
	pDO->Permission = userInfo.getPermission();
	pDO->Role = userInfo.getRole();
	pDO->UserId = brokeid + userid;

	return pDO;
}