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

	std::string server = severName.empty() ? brokerId + ':' + ExchangeRouterTable::TARGET_MD : severName;
	std::string address;
	ExchangeRouterTable::TryFind(server, address);
	pProcessor->InitializeServer(userId, address);

	return ((CTPRawAPI*)pProcessor->getRawAPI())->MdAPI->ReqUserLogin(loginInfo, requestId);
}