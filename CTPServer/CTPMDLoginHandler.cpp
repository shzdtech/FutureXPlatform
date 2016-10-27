/***********************************************************************
 * Module:  CTPMDLoginHandler.cpp
 * Author:  milk
 * Modified: 2015年3月8日 13:12:15
 * Purpose: Implementation of the class CTPMDLoginHandler
 ***********************************************************************/

#include "CTPMDLoginHandler.h"
#include "CTPRawAPI.h"
#include "CTPConstant.h"
#include "CTPMarketDataProcessor.h"
#include "../utility/TUtil.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPMDLoginHandler::LoginFunction()
 // Purpose:    Implementation of CTPMDLoginHandler::LoginFunction()
 // Return:     int
 ////////////////////////////////////////////////////////////////////////

int CTPMDLoginHandler::LoginFunction(ISession* session, CThostFtdcReqUserLoginField* loginInfo, uint requestId, const std::string& severName)
{
	auto pProcessor = (CTPProcessor*)session->getProcessor().get();

	std::string brokerId(loginInfo->BrokerID);
	if (brokerId.empty())
	{
		if (!session->getProcessor()->getServerContext()->getConfigVal(CTP_MD_BROKERID, brokerId))
		{
			brokerId = SysParam::Get(CTP_MD_BROKERID);
		}
		std::strncpy(loginInfo->BrokerID, brokerId.data(), sizeof(loginInfo->BrokerID) - 1);
	}

	std::string userId(loginInfo->UserID);
	if (userId.empty())
	{
		if (!session->getProcessor()->getServerContext()->getConfigVal(CTP_MD_USERID, userId))
		{
			userId = SysParam::Get(CTP_MD_USERID);
		}
		std::strncpy(loginInfo->UserID, userId.data(), sizeof(loginInfo->UserID) - 1);
	}
	
	std::string pwd(loginInfo->Password);
	if (pwd.empty())
	{
		if (!session->getProcessor()->getServerContext()->getConfigVal(CTP_MD_PASSWORD, pwd))
		{
			pwd = SysParam::Get(CTP_MD_PASSWORD);
		}
		std::strncpy(loginInfo->Password, pwd.data(), sizeof(loginInfo->Password) - 1);
	}

	std::string server = severName.empty() ? brokerId + ':' + ExchangeRouterTable::TARGET_MD : severName;
	std::string address;
	ExchangeRouterTable::TryFind(server, address);
	pProcessor->InitializeServer(userId, address);

	return ((CTPRawAPI*)pProcessor->getRawAPI())->MdAPI->ReqUserLogin(loginInfo, requestId);
}