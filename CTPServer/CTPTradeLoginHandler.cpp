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

int CTPTradeLoginHandler::LoginFunction(const IMessageProcessor_Ptr& msgProcessor, CThostFtdcReqUserLoginField* loginInfo, uint requestId, const std::string& severName)
{
	auto pProcessor = (CTPProcessor*)msgProcessor.get();

	std::string brokerId(loginInfo->BrokerID);
	std::string userId(loginInfo->UserID);
	std::string pwd(loginInfo->Password);

	//bool hasPermission = CheckRolePermission(msgProcessor->getMessageSession(), UserRoleType::ROLE_TRADINGDESK, false);
	//if (hasPermission)
	//{
	//	if (brokerId.empty())
	//	{
	//		if (!msgProcessor->getServerContext()->getConfigVal(CTP_TRADER_BROKERID, brokerId))
	//		{
	//			SysParam::TryGet(CTP_TRADER_BROKERID, brokerId);
	//		}
	//		std::strncpy(loginInfo->BrokerID, brokerId.data(), sizeof(loginInfo->BrokerID));
	//	}

	//	if (userId.empty())
	//	{
	//		if (!msgProcessor->getServerContext()->getConfigVal(CTP_TRADER_USERID, userId))
	//		{
	//			SysParam::TryGet(CTP_TRADER_USERID, userId);
	//		}
	//		std::strncpy(loginInfo->UserID, userId.data(), sizeof(loginInfo->UserID));
	//	}

	//	if (pwd.empty())
	//	{
	//		if (!msgProcessor->getServerContext()->getConfigVal(CTP_TRADER_PASSWORD, pwd))
	//		{
	//			SysParam::TryGet(CTP_TRADER_PASSWORD, pwd);
	//		}
	//		std::strncpy(loginInfo->Password, pwd.data(), sizeof(loginInfo->Password));
	//	}
	//}

	std::string address;
	if (!msgProcessor->getServerContext()->getConfigVal(CTP_TRADER_SERVER, address))
	{
		std::string server = severName.empty() ? brokerId + ':' + ExchangeRouterTable::TARGET_TD : severName;
		ExchangeRouterTable::TryFind(server, address);
	}

	pProcessor->CreateCTPAPI(userId, address);
	int ret = pProcessor->RawAPI_Ptr()->TdAPI->ReqUserLogin(loginInfo, requestId);

	// try after market server
	if (ret == -1 && severName.empty())
	{
		std::string server = brokerId + ':' + ExchangeRouterTable::TARGET_TD_AM;
		if (ExchangeRouterTable::TryFind(server, address))
		{
			pProcessor->CreateCTPAPI(userId, address);
			ret = pProcessor->RawAPI_Ptr()->TdAPI->ReqUserLogin(loginInfo, requestId);
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
	((CTPRawAPI*)rawAPI)->TdAPI->ReqSettlementInfoConfirm(&reqsettle, 0);

	CThostFtdcQryInvestorPositionField req{};
	((CTPRawAPI*)rawAPI)->TdAPI->ReqQryInvestorPosition(&req, -1);
	std::this_thread::sleep_for(std::chrono::seconds(1));

	return ret;
}
