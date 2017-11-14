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
 // Name:       CTPTradeLoginHandler::LoginFunction()
 // Purpose:    Implementation of CTPTradeLoginHandler::LoginFunction()
 // Return:     int
 ////////////////////////////////////////////////////////////////////////

int CTPTradeLoginHandler::LoginFunction(const IMessageProcessor_Ptr& msgProcessor, CThostFtdcReqUserLoginField* loginInfo, uint requestId, const std::string& serverName)
{
	auto pProcessor = (CTPProcessor*)msgProcessor.get();

	int ret = 0;

	std::string server = serverName;
	if (server.empty())
		msgProcessor->getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_TD, server);

	ExchangeRouterDO exDO;
	ExchangeRouterTable::TryFind(server, exDO);

	pProcessor->CreateCTPAPI(loginInfo->UserID, exDO.Address);

	CThostFtdcReqAuthenticateField reqAuth{};

	if (!exDO.AuthCode.empty())
	{
		std::strncpy(reqAuth.BrokerID, exDO.BrokeID.data(), sizeof(reqAuth.BrokerID));
		std::strncpy(reqAuth.UserID, loginInfo->UserID, sizeof(reqAuth.UserID));
		std::strncpy(reqAuth.AuthCode, exDO.AuthCode.data(), sizeof(reqAuth.AuthCode));
		std::strncpy(reqAuth.UserProductInfo, exDO.ProductInfo.data(), sizeof(reqAuth.UserProductInfo));

		ret = pProcessor->RawAPI_Ptr()->TdAPIProxy()->get()->ReqAuthenticate(&reqAuth, 0);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if(ret == 0)
		ret = pProcessor->RawAPI_Ptr()->TdAPIProxy()->get()->ReqUserLogin(loginInfo, requestId);

	// try after market server
	if (ret == -1)
	{
		msgProcessor->getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_TD_AM, server);
		if (ExchangeRouterTable::TryFind(server, exDO))
		{
			pProcessor->CreateCTPAPI(loginInfo->UserID, exDO.Address);
			if (exDO.AuthCode.empty())
			{
				ret = pProcessor->RawAPI_Ptr()->TdAPIProxy()->get()->ReqAuthenticate(&reqAuth, 0);
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			ret = pProcessor->RawAPI_Ptr()->TdAPIProxy()->get()->ReqUserLogin(loginInfo, requestId);
		}
	}

	return ret;
}

dataobj_ptr CTPTradeLoginHandler::HandleResponse(const uint32_t serialId, const param_vector & rawRespParams, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	dataobj_ptr ret = CTPLoginHandler::HandleResponse(serialId, rawRespParams, rawAPI, msgProcessor, session);

	CThostFtdcSettlementInfoConfirmField reqsettle{};
	std::strncpy(reqsettle.BrokerID, session->getUserInfo().getBrokerId().data(), sizeof(reqsettle.BrokerID));
	std::strncpy(reqsettle.InvestorID, session->getUserInfo().getInvestorId().data(), sizeof(reqsettle.InvestorID));
	((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqSettlementInfoConfirm(&reqsettle, 0);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		pWorkerProc->RegisterLoggedSession(session);
		
		if (pWorkerProc->IsLoadPositionFromDB())
		{
			pWorkerProc->LoadPositonFromDatabase(session->getUserInfo().getUserId(), std::to_string(session->getUserInfo().getTradingDay()));
		}
		else
		{
			CThostFtdcQryInvestorPositionField req{};
			((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqQryInvestorPosition(&req, -1);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	return ret;
}
