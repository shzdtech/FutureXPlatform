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
#include "../litelogger/LiteLogger.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTSLoginHandler::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTSLoginHandler::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSLoginHandler::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& brokeid = stdo->TryFind(STR_BROKER_ID, EMPTY_STRING);
	auto& userid = stdo->TryFind(STR_USER_NAME, EMPTY_STRING);
	auto& password = stdo->TryFind(STR_PASSWORD, EMPTY_STRING);

	CTSAPIWrapper* api = (CTSAPIWrapper*)rawAPI;
	api->Impl()->Login(brokeid.data(), userid.data(), password.data(), reqDO->SerialId);

	LOG_DEBUG << "Start login: " << brokeid << ":" << userid << ":" << password;

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
// Name:       CTSLoginHandler::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSLoginHandler::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSLoginHandler::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
{
	auto errCode = *(int*)rawParams[0];
	if (errCode)
	{
		throw BizException(errCode);
	}

	auto pDO = new UserInfoDO;
	dataobj_ptr ret(pDO);

	auto pUserInfo = session->getUserInfo();

	pDO->UserId = pUserInfo->getUserId();
	pDO->BrokerId = pUserInfo->getBrokerId();
	//pDO->Password = pUserInfo->getPassword();
	pDO->UserName = pUserInfo->getName();
	pDO->Role = pUserInfo->getRole();
	pDO->Permission = pUserInfo->getPermission();

	session->setLoginTimeStamp();

	LOG_DEBUG << pDO->UserId << " login successful.";

	return ret;
}