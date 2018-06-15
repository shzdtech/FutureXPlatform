/***********************************************************************
 * Module:  XTTradeLoginHandler.cpp
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Implementation of the class XTTradeLoginHandler
 ***********************************************************************/

#include "XTTradeLoginHandler.h"
#include "XTTradeProcessor.h"
#include "XTRawAPI.h"
#include "XTConstant.h"
#include "XTTradeWorkerProcessor.h"
#include "XTUtility.h"

#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../utility/TUtil.h"
#include "../common/Attribute_Key.h"
#include "../dataobject/TypedefDO.h"
#include "../litelogger/LiteLogger.h"
#include "../bizutility/PositionPortfolioMap.h"
#include "../bizutility/ModelParamDefCache.h"
#include "../bizutility/ModelParamsCache.h"
#include "../riskmanager/RiskModelAlgorithmManager.h"
#include "../riskmanager/RiskContext.h"
#include "../riskmanager/RiskModelParams.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       XTTradeLoginHandler::LoginFromServer()
 // Purpose:    Implementation of XTTradeLoginHandler::LoginFromServer()
 // Return:     int
 ////////////////////////////////////////////////////////////////////////

std::shared_ptr<UserInfoDO> XTTradeLoginHandler::LoginFromServer(const CTPProcessor_Ptr& msgProcessor,
	const std::shared_ptr<UserInfoDO> & userInfoDO_Ptr, uint requestId, const std::string& serverName)
{
	auto& session = msgProcessor->getMessageSession();
	auto& userInfo = session->getUserInfo();

	PositionPortfolioMap::LoadUserPortfolio(userInfo.getUserId());

	if (auto pWorkerProc = GetWorkerProcessor(msgProcessor))
	{
		pWorkerProc->RegisterLoggedSession(session);
	}

	LoginFromDB(msgProcessor);

	LoadUserRiskModel(msgProcessor);

	if (userInfoDO_Ptr->ExchangeUser.empty())
	{
		msgProcessor->getMessageSession()->setLoginTimeStamp();
		return userInfoDO_Ptr;
	}

	((XTRawAPI*)msgProcessor->getRawAPI())->get()->userLogin(userInfoDO_Ptr->ExchangeUser.data(), userInfoDO_Ptr->ExchangePassword.data(), requestId);

	return nullptr;
}

dataobj_ptr XTTradeLoginHandler::HandleResponse(const uint32_t serialId, const param_vector & rawRespParams, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	XTUtility::CheckError(rawRespParams[3]);
	auto& userInfo = session->getUserInfo();
	userInfo.setBrokerId((char*)rawRespParams[0]);
	userInfo.setPassword((char*)rawRespParams[1]);
	return std::static_pointer_cast<UserInfoDO>(userInfo.getExtInfo());
}
