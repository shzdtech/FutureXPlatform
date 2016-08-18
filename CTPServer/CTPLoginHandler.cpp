/***********************************************************************
 * Module:  CTPLoginHandler.cpp
 * Author:  milk
 * Modified: 2014年10月6日 20:07:26
 * Purpose: Implementation of the class CTPLoginHandler
 ***********************************************************************/


#include "CTPLoginHandler.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../dataobject/UserInfoDO.h"

#include "../message/BizError.h"
#include "../message/UserInfo.h"

#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include "../litelogger/LiteLogger.h"

#include "CTPUtility.h"
#include "CTPConstant.h"

#include "../common/Attribute_Key.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPLoginHandler::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor_Ptr session)
// Purpose:    Implementation of CTPLoginHandler::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPLoginHandler::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	if (!session->getLoginTimeStamp())
	{

		auto stdo = (MapDO<std::string>*)reqDO.get();

		auto& brokeid = stdo->TryFind(STR_BROKER_ID, EMPTY_STRING);
		auto& userid = stdo->TryFind(STR_USER_NAME, EMPTY_STRING);
		auto& password = stdo->TryFind(STR_PASSWORD, EMPTY_STRING);

		CThostFtdcReqUserLoginField req{};

		std::strcpy(req.BrokerID, brokeid.data());
		std::strcpy(req.UserID, userid.data());
		std::strcpy(req.Password, password.data());
		std::strcpy(req.UserProductInfo, UUID_MICROFUTURE_CTP);

		int ret = LoginFunction(rawAPI, session, &req, stdo->SerialId);
		CTPUtility::CheckReturnError(ret);
		//int ret = ((CThostFtdcMdApi*)rawAPI)->ReqUserLogin(&req, 1);

		auto pUserInfo = session->getUserInfo();
		pUserInfo->setInvestorId(req.UserID);
		pUserInfo->setBrokerId(req.BrokerID);
		pUserInfo->setName(userid);
		pUserInfo->setPassword(password);
		pUserInfo->setUserId(req.UserID);
		pUserInfo->setRole(ROLE_CLIENT);
		pUserInfo->setPermission(ALLOW_TRADING);

		LOG_DEBUG << "Login: " << req.BrokerID << ":" << userid << ":" << password;
	}

	auto userInfoDO_Ptr = std::static_pointer_cast<UserInfoDO>
		(session->getUserInfo()->getAttribute(STR_KEY_USER_INFO_DETAIL));

	return userInfoDO_Ptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPLoginHandler::HandleResponse(const uint32_t serialId, void* bizObj, IRawAPI* rawAPI, IMessageProcessor_Ptr session)
// Purpose:    Implementation of CTPLoginHandler::HandleResponse(const uint32_t serialId, )
// Parameters:
// - bizObj
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPLoginHandler::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);

	auto pData = (CThostFtdcRspUserLoginField*)rawRespParams[0];
	
	auto pDO = new UserInfoDO;
	dataobj_ptr ret(pDO);

	auto pUserInfo = session->getUserInfo();

	unsigned long initVal = std::strtoul(pData->MaxOrderRef, nullptr, 0) + 1;
	pUserInfo->setInitSeq(initVal);
	pUserInfo->setFrontId(pData->FrontID);
	pUserInfo->setSessionId(pData->SessionID);

	pDO->BrokerId = pUserInfo->getBrokerId();
	pDO->Company = pUserInfo->getBrokerId();
	pDO->UserName = pUserInfo->getName();
	//pDO->Password = pUserInfo->getPassword();
	pDO->Permission = pUserInfo->getPermission();
	pDO->Role = pUserInfo->getRole();
	pDO->UserId = pUserInfo->getUserId();

	session->getUserInfo()->setAttribute(STR_KEY_USER_INFO_DETAIL, ret);
	session->setLoginTimeStamp();

	LOG_DEBUG << pDO->UserId << " login successful.";

	return ret;
}