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
#include "../bizutility/ExchangeRouterTable.h"
#include "../utility/TUtil.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPMDLoginHandler::LoginFunction()
 // Purpose:    Implementation of CTPMDLoginHandler::LoginFunction()
 // Return:     int
 ////////////////////////////////////////////////////////////////////////

int CTPMDLoginHandler::LoginFunction(const IMessageProcessor_Ptr& msgProcessor, CThostFtdcReqUserLoginField* loginInfo, uint requestId, const std::string& serverName)
{
	auto pProcessor = (CTPProcessor*)msgProcessor.get();

	/*if (!msgProcessor->getServerContext()->getConfigVal(CTP_MD_SERVER, address))
	{
		std::string server = serverName.empty() ? brokerId + ':' + ExchangeRouterTable::TARGET_MD : serverName;
		ExchangeRouterTable::TryFind(server, address);
	}*/

	std::string server = serverName;
	if (server.empty())
		msgProcessor->getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_MD, server);

	ExchangeRouterDO exDO;
	ExchangeRouterTable::TryFind(server, exDO);

	pProcessor->CreateCTPAPI(exDO.UserID, exDO.Address);
	int ret = pProcessor->RawAPI_Ptr()->MdAPIProxy()->get()->ReqUserLogin(loginInfo, requestId);

	// try after market server
	if (ret == -1)
	{
		msgProcessor->getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_MD_AM, server);
		if (ExchangeRouterTable::TryFind(server, exDO))
		{
			pProcessor->CreateCTPAPI(exDO.UserID, exDO.Address);
			ret = pProcessor->RawAPI_Ptr()->MdAPIProxy()->get()->ReqUserLogin(loginInfo, requestId);
		}
	}

	return ret;
}