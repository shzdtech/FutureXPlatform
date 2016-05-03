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
#include <glog/logging.h>

#include "CTPUtility.h"
#include "CTPConstant.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPLoginHandler::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, IMessageProcessor_Ptr session)
// Purpose:    Implementation of CTPLoginHandler::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPLoginHandler::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();
	auto& data = stdo->Data;
	
	auto& brokeid = TUtil::FirstNamedEntry(STR_BROKER_ID, data, EMPTY_STRING);
	auto& userid = TUtil::FirstNamedEntry(STR_USER_ID, data, EMPTY_STRING);
	auto& password = TUtil::FirstNamedEntry(STR_PASSWORD, data, EMPTY_STRING);

	CThostFtdcReqUserLoginField req;
	std::memset(&req, 0, sizeof(req));

	std::strcpy(req.BrokerID, brokeid.data());
	std::strcpy(req.UserID, userid.data());
	std::strcpy(req.Password, password.data());
	std::strcpy(req.UserProductInfo, UUID_MICROFUTURE_CTP);

	int ret = LoginFunction(rawAPI, &req);
	CTPUtility::CheckReturnError(ret);
	//int ret = ((CThostFtdcMdApi*)rawAPI)->ReqUserLogin(&req, 1);

	auto pUserInfo = session->getUserInfo();
	pUserInfo->setInvestorId(req.UserID);
	pUserInfo->setBrokerId(req.BrokerID);
	pUserInfo->setCompany(userid);
	pUserInfo->setContactNum(EMPTY_STRING);
	pUserInfo->setEmail(EMPTY_STRING);
	pUserInfo->setName(userid);
	pUserInfo->setPassword(password);
	pUserInfo->setUserId(userid);
	pUserInfo->setRole(ROLE_CLIENT);
	pUserInfo->setPermission(ALLOW_TRADING);

	DLOG(INFO) << "Login: " << req.BrokerID << ":" << userid << ":" << password << std::endl;

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPLoginHandler::HandleResponse(void* bizObj, IRawAPI* rawAPI, IMessageProcessor_Ptr session)
// Purpose:    Implementation of CTPLoginHandler::HandleResponse()
// Parameters:
// - bizObj
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPLoginHandler::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
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
	pDO->Company = pUserInfo->getCompany();
	pDO->ContactNum = pUserInfo->getContactNum();
	pDO->Email = pUserInfo->getEmail();
	pDO->Name = pUserInfo->getName();
	//pDO->Password = pUserInfo->getPassword();
	pDO->Permission = pUserInfo->getPermission();
	pDO->Role = pUserInfo->getRole();
	pDO->UserId = pUserInfo->getUserId();

	session->setLoginStatus(true);

	DLOG(INFO) << "Login successful." << std::endl;

	return ret;
}