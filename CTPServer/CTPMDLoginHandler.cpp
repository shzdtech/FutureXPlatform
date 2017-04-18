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

int CTPMDLoginHandler::LoginFunction(const IMessageProcessor_Ptr& msgProcessor, CThostFtdcReqUserLoginField* loginInfo, uint requestId, const std::string& severName)
{
	auto pProcessor = (CTPProcessor*)msgProcessor.get();

	std::string brokerId(loginInfo->BrokerID);
	if (brokerId.empty())
	{
		if (!msgProcessor->getServerContext()->getConfigVal(CTP_MD_BROKERID, brokerId))
		{
			SysParam::TryGet(CTP_MD_BROKERID, brokerId);
		}
		std::strncpy(loginInfo->BrokerID, brokerId.data(), sizeof(loginInfo->BrokerID));
	}

	std::string userId(loginInfo->UserID);
	if (userId.empty())
	{
		if (!msgProcessor->getServerContext()->getConfigVal(CTP_MD_USERID, userId))
		{
			SysParam::TryGet(CTP_MD_USERID, userId);
		}
		std::strncpy(loginInfo->UserID, userId.data(), sizeof(loginInfo->UserID));
	}
	
	std::string pwd(loginInfo->Password);
	if (pwd.empty())
	{
		if (!msgProcessor->getServerContext()->getConfigVal(CTP_MD_PASSWORD, pwd))
		{
			SysParam::TryGet(CTP_MD_PASSWORD, pwd);
		}
		std::strncpy(loginInfo->Password, pwd.data(), sizeof(loginInfo->Password));
	}

	std::string address;
	if (!msgProcessor->getServerContext()->getConfigVal(CTP_MD_SERVER, address))
	{
		std::string server = severName.empty() ? brokerId + ':' + ExchangeRouterTable::TARGET_MD : severName;
		ExchangeRouterTable::TryFind(server, address);
	}

	pProcessor->CreateCTPAPI(userId, address);
	int ret = pProcessor->RawAPI_Ptr()->MdAPI->ReqUserLogin(loginInfo, requestId);

	// try after market server
	if (ret == -1 && severName.empty())
	{
		std::string server = brokerId + ':' + ExchangeRouterTable::TARGET_MD_AM;
		if (ExchangeRouterTable::TryFind(server, address))
		{
			pProcessor->CreateCTPAPI(userId, address);
			ret = pProcessor->RawAPI_Ptr()->MdAPI->ReqUserLogin(loginInfo, requestId);
		}
	}

	return ret;
}