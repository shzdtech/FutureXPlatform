/***********************************************************************
 * Module:  CTPTradeLoginHandler.cpp
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Implementation of the class CTPTradeLoginHandler
 ***********************************************************************/

#include "CTPTradeLoginHandler.h"
#include "CTPProcessor.h"
#include "CTPRawAPI.h"
#include "CTPConstant.h"
#include "CTPTradeWorkerProcessor.h"

#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../utility/TUtil.h"
#include "../common/Attribute_Key.h"
#include "../dataobject/TypedefDO.h"
#include "../litelogger/LiteLogger.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeLoginHandler::LoginFunction()
// Purpose:    Implementation of CTPTradeLoginHandler::LoginFunction()
// Return:     int
////////////////////////////////////////////////////////////////////////

int CTPTradeLoginHandler::LoginFunction(const IMessageProcessor_Ptr& msgProcessor, CThostFtdcReqUserLoginField* loginInfo, uint requestId, const std::string& severName)
{
	auto pProcessor = (CTPProcessor*)msgProcessor.get();

	std::string brokerId(loginInfo->BrokerID);
	if (brokerId.empty())
	{
		if (!msgProcessor->getServerContext()->getConfigVal(CTP_TRADER_BROKERID, brokerId))
		{
			SysParam::TryGet(CTP_TRADER_BROKERID, brokerId);
		}
		std::strncpy(loginInfo->BrokerID, brokerId.data(), sizeof(loginInfo->BrokerID));
	}

	std::string userId(loginInfo->UserID);
	if (userId.empty())
	{
		if (!msgProcessor->getServerContext()->getConfigVal(CTP_TRADER_USERID, userId))
		{
			SysParam::TryGet(CTP_TRADER_USERID, userId);
		}
		std::strncpy(loginInfo->UserID, userId.data(), sizeof(loginInfo->UserID));
	}

	std::string pwd(loginInfo->Password);
	if (pwd.empty())
	{
		if (!msgProcessor->getServerContext()->getConfigVal(CTP_TRADER_PASSWORD, pwd))
		{
			SysParam::TryGet(CTP_TRADER_PASSWORD, pwd);
		}
		std::strncpy(loginInfo->Password, pwd.data(), sizeof(loginInfo->Password));
	}

	std::string server = severName.empty() ? brokerId + ':' + ExchangeRouterTable::TARGET_TD : severName;
	std::string address;
	ExchangeRouterTable::TryFind(server, address);
	pProcessor->InitializeServer(userId, address);


	return ((CTPRawAPI*)pProcessor->getRawAPI())->TrdAPI->ReqUserLogin(loginInfo, requestId);
}

dataobj_ptr CTPTradeLoginHandler::HandleResponse(const uint32_t serialId, const param_vector & rawRespParams, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	dataobj_ptr ret = CTPLoginHandler::HandleResponse(serialId, rawRespParams, rawAPI, msgProcessor, session);

	CThostFtdcSettlementInfoConfirmField reqsettle{};
	std::strncpy(reqsettle.BrokerID, session->getUserInfo().getBrokerId().data(), sizeof(reqsettle.BrokerID));
	std::strncpy(reqsettle.InvestorID, session->getUserInfo().getInvestorId().data(), sizeof(reqsettle.InvestorID));
	((CTPRawAPI*)rawAPI)->TrdAPI->ReqSettlementInfoConfirm(&reqsettle, 0);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		auto& userInfo = session->getUserInfo();
		auto positionMap = pWorkerProc->GetUserPositionContext().GetPositionsByUser(userInfo.getUserId());
		if (positionMap.empty())
		{
			CThostFtdcQryInvestorPositionField req{};
			((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryInvestorPosition(&req, serialId);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	return ret;
}
