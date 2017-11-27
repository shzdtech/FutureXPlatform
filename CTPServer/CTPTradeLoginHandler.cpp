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
#include "CTPUtility.h"

#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../utility/TUtil.h"
#include "../common/Attribute_Key.h"
#include "../dataobject/TypedefDO.h"
#include "../litelogger/LiteLogger.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPTradeLoginHandler::LoginFromServer()
 // Purpose:    Implementation of CTPTradeLoginHandler::LoginFromServer()
 // Return:     int
 ////////////////////////////////////////////////////////////////////////

std::shared_ptr<UserInfoDO> CTPTradeLoginHandler::LoginFromServer(const IMessageProcessor_Ptr& msgProcessor, const std::shared_ptr<UserInfoDO>& userInfo_Ptr,
	uint requestId, const std::string& serverName)
{
	auto pProcessor = (CTPProcessor*)msgProcessor.get();

	if (userInfo_Ptr->LoadFromDB)
	{
		LoginFromDB(msgProcessor);
		pProcessor->getMessageSession()->getUserInfo().setExtInfo(userInfo_Ptr);
		pProcessor->getMessageSession()->setLoginTimeStamp();
		return userInfo_Ptr;
	}

	int ret = 0;

	std::string server = serverName;
	if (server.empty())
		msgProcessor->getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_TD, server);

	ExchangeRouterDO exDO;
	ExchangeRouterTable::TryFind(server, exDO);

	auto& userInfo = msgProcessor->getMessageSession()->getUserInfo();

	CThostFtdcReqUserLoginField req{};
	std::strncpy(req.BrokerID, userInfo.getBrokerId().data(), sizeof(req.BrokerID));
	std::strncpy(req.UserID, userInfo.getInvestorId().data(), sizeof(req.UserID));
	std::strncpy(req.Password, userInfo.getPassword().data(), sizeof(req.Password));

	pProcessor->CreateCTPAPI(userInfo.getInvestorId(), exDO.Address);

	CThostFtdcReqAuthenticateField reqAuth{};

	if (!exDO.AuthCode.empty())
	{
		std::strncpy(reqAuth.BrokerID, exDO.BrokeID.data(), sizeof(reqAuth.BrokerID));
		std::strncpy(reqAuth.UserID, userInfo.getInvestorId().data(), sizeof(reqAuth.UserID));
		std::strncpy(reqAuth.AuthCode, exDO.AuthCode.data(), sizeof(reqAuth.AuthCode));
		std::strncpy(reqAuth.UserProductInfo, exDO.ProductInfo.data(), sizeof(reqAuth.UserProductInfo));

		ret = pProcessor->RawAPI_Ptr()->TdAPIProxy()->get()->ReqAuthenticate(&reqAuth, 0);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if(ret == 0)
		ret = pProcessor->RawAPI_Ptr()->TdAPIProxy()->get()->ReqUserLogin(&req, requestId);

	// try after market server
	if (ret == -1)
	{
		msgProcessor->getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_TD_AM, server);
		if (ExchangeRouterTable::TryFind(server, exDO))
		{
			pProcessor->CreateCTPAPI(userInfo.getInvestorId(), exDO.Address);
			if (exDO.AuthCode.empty())
			{
				ret = pProcessor->RawAPI_Ptr()->TdAPIProxy()->get()->ReqAuthenticate(&reqAuth, 0);
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			ret = pProcessor->RawAPI_Ptr()->TdAPIProxy()->get()->ReqUserLogin(&req, requestId);
		}
	}

	CTPUtility::CheckReturnError(ret);

	return nullptr;
}

bool CTPTradeLoginHandler::LoginFromDB(const IMessageProcessor_Ptr& msgProcessor)
{
	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		auto session = msgProcessor->getMessageSession();

		pWorkerProc->RegisterLoggedSession(session);

		if (pWorkerProc->IsLoadPositionFromDB())
		{
			pWorkerProc->LoadPositonFromDatabase(session->getUserInfo().getUserId(),
				session->getUserInfo().getInvestorId(), std::to_string(session->getUserInfo().getTradingDay()));

			((CTPProcessor*)msgProcessor.get())->DataLoadMask |= CTPProcessor::POSITION_DATA_LOADED;
		}
	}

	return true;
}

dataobj_ptr CTPTradeLoginHandler::HandleResponse(const uint32_t serialId, const param_vector & rawRespParams, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	dataobj_ptr ret = CTPLoginHandler::HandleResponse(serialId, rawRespParams, rawAPI, msgProcessor, session);

	CThostFtdcSettlementInfoConfirmField reqsettle{};
	std::strncpy(reqsettle.BrokerID, session->getUserInfo().getBrokerId().data(), sizeof(reqsettle.BrokerID));
	std::strncpy(reqsettle.InvestorID, session->getUserInfo().getInvestorId().data(), sizeof(reqsettle.InvestorID));
	((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqSettlementInfoConfirm(&reqsettle, 0);

	LoginFromDB(msgProcessor);

	CThostFtdcQryInvestorPositionField req{};
	((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqQryInvestorPosition(&req, -1);
	std::this_thread::sleep_for(std::chrono::seconds(1));

	return ret;
}
