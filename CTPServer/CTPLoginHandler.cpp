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

#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"
#include "../litelogger/LiteLogger.h"

#include "CTPProcessor.h"
#include "CTPUtility.h"
#include "CTPConstant.h"

#include "../common/Attribute_Key.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPLoginHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor_Ptr session)
// Purpose:    Implementation of CTPLoginHandler::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPLoginHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	if (!session->getLoginTimeStamp())
	{
		auto pProcessor = (CTPProcessor*)msgProcessor.get();;

		auto stdo = (MapDO<std::string>*)reqDO.get();

		auto& brokeid = stdo->TryFind(STR_BROKER_ID, EMPTY_STRING);
		auto& userid = stdo->TryFind(STR_USER_NAME, EMPTY_STRING);
		auto& password = stdo->TryFind(STR_PASSWORD, EMPTY_STRING);
		auto& routername = stdo->TryFind(STR_ROUTER_NAME, EMPTY_STRING);

		CThostFtdcReqUserLoginField req{};

		std::strncpy(req.BrokerID, brokeid.data(), sizeof(req.BrokerID));
		std::strncpy(req.UserID, userid.data(), sizeof(req.UserID));
		std::strncpy(req.Password, password.data(), sizeof(req.Password));
		// std::strcpy(req.UserProductInfo, UUID_MICROFUTURE_CTP);

		pProcessor->LoginSerialId = serialId;
		int ret = LoginFunction(msgProcessor, &req, pProcessor->LoginSerialId, routername);
		CTPUtility::CheckReturnError(ret);
		//int ret = ((CThostFtdcMdApi*)rawAPI)->ReqUserLogin(&req, 1);

		auto& userInfo = session->getUserInfo();
		userInfo.setInvestorId(req.UserID);
		userInfo.setBrokerId(req.BrokerID);
		userInfo.setName(userid);
		userInfo.setPassword(password);

		userInfo.setUserId(CTPUtility::MakeUserID(req.BrokerID, req.UserID));
		userInfo.setRole(ROLE_CLIENT);
		userInfo.setPermission(ALLOW_TRADING);

		LOG_DEBUG << "Login: " << req.BrokerID << ":" << userid << ":" << password;
	}

	auto userInfoDO_Ptr = std::static_pointer_cast<UserInfoDO>(session->getUserInfo().getExtInfo());

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

dataobj_ptr CTPLoginHandler::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckError(rawRespParams[1]);

	auto pData = (CThostFtdcRspUserLoginField*)rawRespParams[0];
	
	auto pDO = new UserInfoDO;
	dataobj_ptr ret(pDO);

	auto& userInfo = session->getUserInfo();

	unsigned long initVal = std::strtoul(pData->MaxOrderRef, nullptr, 0) + 1;
	userInfo.setInitSeq(initVal);
	userInfo.setFrontId(pData->FrontID);
	userInfo.setSessionId(pData->SessionID);
	userInfo.setTradingDay(std::atoi(pData->TradingDay));

	pDO->BrokerId = userInfo.getBrokerId();
	pDO->Company = userInfo.getBrokerId();
	pDO->UserName = userInfo.getName();
	//pDO->Password = userInfo.getPassword();
	pDO->Permission = userInfo.getPermission();
	pDO->Role = userInfo.getRole();
	pDO->UserId = userInfo.getUserId();

	session->getUserInfo().setExtInfo(ret);
	session->setLoginTimeStamp();

	LOG_DEBUG << pDO->UserId << " login successful.";

	return ret;
}