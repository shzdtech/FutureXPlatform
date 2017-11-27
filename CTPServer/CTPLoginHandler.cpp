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
#include "../databaseop/UserInfoDAO.h"
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
	auto pProcessor = (CTPProcessor*)msgProcessor.get();;

	auto stdo = (StringMapDO<std::string>*)reqDO.get();

	std::string server;
	msgProcessor->getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_TD, server);
	ExchangeRouterDO exDO;
	if (!ExchangeRouterTable::TryFind(server, exDO))
	{
		msgProcessor->getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_MD, server);
		ExchangeRouterTable::TryFind(server, exDO);
	}

	// auto& brokeid = stdo->TryFind(STR_BROKER_ID, EMPTY_STRING);
	auto& userid = stdo->TryFind(STR_USER_NAME, EMPTY_STRING);
	auto& password = stdo->TryFind(STR_PASSWORD, EMPTY_STRING);
	auto& routername = stdo->TryFind(STR_ROUTER_NAME, EMPTY_STRING);

	auto& userInfo = session->getUserInfo();
	userInfo.setInvestorId(userid);
	userInfo.setBrokerId(exDO.BrokeID);
	userInfo.setPassword(password);

	std::shared_ptr<UserInfoDO> userInfo_Ptr;
	if (userInfo_Ptr = UserInfoDAO::FindUser(userid))
	{
		if (!userInfo_Ptr->Password.empty())
		{
			userInfo.setInvestorId(exDO.UserID);
			userInfo.setBrokerId(exDO.BrokeID);
			userInfo_Ptr->LoadFromDB = true;
		}
	}
	else
	{
		userInfo_Ptr = UserInfoDAO::FindUser(CTPUtility::MakeUserID(exDO.BrokeID, userid));
		if (!userInfo_Ptr)
			throw UserException(USERID_NOT_EXITS, "UserId: " + exDO.BrokeID + userid + " not exists.");
	}

	userInfo.setUserId(userInfo_Ptr->UserId);
	userInfo.setName(userInfo_Ptr->UserName);
	userInfo.setRole(userInfo_Ptr->Role);
	userInfo.setPermission(userInfo_Ptr->Permission);


	pProcessor->LoginSerialId = serialId;
	auto ret = LoginFromServer(msgProcessor, userInfo_Ptr, pProcessor->LoginSerialId, routername);

	LOG_DEBUG << "Login: " << userInfo.getBrokerId() << ":" << userid << ":" << password;

	return ret;
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