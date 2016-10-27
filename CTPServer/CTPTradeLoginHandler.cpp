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
#include "../utility/TUtil.h"
#include "../common/Attribute_Key.h"
#include "../dataobject/TypedefDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeLoginHandler::LoginFunction()
// Purpose:    Implementation of CTPTradeLoginHandler::LoginFunction()
// Return:     int
////////////////////////////////////////////////////////////////////////

int CTPTradeLoginHandler::LoginFunction(ISession* session, CThostFtdcReqUserLoginField* loginInfo, uint requestId, const std::string& severName)
{
	auto pProcessor = (CTPProcessor*)session->getProcessor().get();

	std::string brokerId(loginInfo->BrokerID);
	if (brokerId.empty())
	{
		if (!session->getProcessor()->getServerContext()->getConfigVal(CTP_TRADER_BROKERID, brokerId))
		{
			brokerId = SysParam::Get(CTP_TRADER_BROKERID);
		}
		std::strncpy(loginInfo->BrokerID, brokerId.data(), sizeof(loginInfo->BrokerID) - 1);
	}

	std::string userId(loginInfo->UserID);
	if (userId.empty())
	{
		if (!session->getProcessor()->getServerContext()->getConfigVal(CTP_TRADER_BROKERID, userId))
		{
			userId = SysParam::Get(CTP_TRADER_BROKERID);
		}
		std::strncpy(loginInfo->UserID, userId.data(), sizeof(loginInfo->UserID) - 1);
	}

	std::string pwd(loginInfo->Password);
	if (pwd.empty())
	{
		if (!session->getProcessor()->getServerContext()->getConfigVal(CTP_TRADER_USERID, pwd))
		{
			pwd = SysParam::Get(CTP_TRADER_USERID);
		}
		std::strncpy(loginInfo->Password, pwd.data(), sizeof(loginInfo->Password) - 1);
	}

	std::string server = severName.empty() ? brokerId + ':' + ExchangeRouterTable::TARGET_TD : severName;
	std::string address;
	ExchangeRouterTable::TryFind(server, address);
	pProcessor->InitializeServer(userId, address);


	return ((CTPRawAPI*)pProcessor->getRawAPI())->TrdAPI->ReqUserLogin(loginInfo, requestId);
}

dataobj_ptr CTPTradeLoginHandler::HandleResponse(const uint32_t serialId, const param_vector & rawRespParams, IRawAPI * rawAPI, ISession * session)
{
	dataobj_ptr ret = CTPLoginHandler::HandleResponse(serialId, rawRespParams, rawAPI, session);

	CThostFtdcSettlementInfoConfirmField reqsettle{};
	std::strncpy(reqsettle.BrokerID, session->getUserInfo()->getBrokerId().data(), sizeof(reqsettle.BrokerID) - 1);
	std::strncpy(reqsettle.InvestorID, session->getUserInfo()->getInvestorId().data(), sizeof(reqsettle.InvestorID) - 1);
	((CTPRawAPI*)rawAPI)->TrdAPI->ReqSettlementInfoConfirm(&reqsettle, 0);

	return ret;
}
