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
#include "CTPUtility.h"
#include "../bizutility/ExchangeRouterTable.h"
#include "../utility/TUtil.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPMDLoginHandler::LoginFromServer()
 // Purpose:    Implementation of CTPMDLoginHandler::LoginFromServer()
 // Return:     int
 ////////////////////////////////////////////////////////////////////////

std::shared_ptr<UserInfoDO> CTPMDLoginHandler::LoginFromServer(const CTPProcessor_Ptr& msgProcessor,
	const std::shared_ptr<UserInfoDO> & userInfoDO_Ptr, uint requestId, const std::string& serverName)
{
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

	auto mktProc_Ptr = std::static_pointer_cast<CTPMarketDataProcessor>(msgProcessor);

	mktProc_Ptr->CreateBackendAPI(mktProc_Ptr.get(), exDO.UserID, exDO.Address);

	auto& userInfo = msgProcessor->getMessageSession()->getUserInfo();

	CThostFtdcReqUserLoginField req{};
	std::strncpy(req.BrokerID, exDO.BrokeID.data(), sizeof(req.BrokerID));
	std::strncpy(req.UserID, userInfo.getInvestorId().data(), sizeof(req.UserID));
	std::strncpy(req.Password, userInfo.getPassword().data(), sizeof(req.Password));
	// std::strcpy(req.UserProductInfo, UUID_MICROFUTURE_CTP)
	int ret = ((CTPRawAPI*)msgProcessor->getRawAPI())->MdAPIProxy()->get()->ReqUserLogin(&req, requestId);

	// try after market server
	if (ret == -1)
	{
		msgProcessor->getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_MD_AM, server);
		if (ExchangeRouterTable::TryFind(server, exDO))
		{
			mktProc_Ptr->CreateBackendAPI(mktProc_Ptr.get(), exDO.UserID, exDO.Address);
			ret = ((CTPRawAPI*)msgProcessor->getRawAPI())->MdAPIProxy()->get()->ReqUserLogin(&req, requestId);
		}
	}

	CTPUtility::CheckReturnError(ret);

	return nullptr;
}