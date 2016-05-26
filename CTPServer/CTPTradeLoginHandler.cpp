/***********************************************************************
 * Module:  CTPTradeLoginHandler.cpp
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Implementation of the class CTPTradeLoginHandler
 ***********************************************************************/

#include "CTPTradeLoginHandler.h"
#include "CTPRawAPI.h"
#include "CTPConstant.h"
#include "../utility/TUtil.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeLoginHandler::LoginFunction()
// Purpose:    Implementation of CTPTradeLoginHandler::LoginFunction()
// Return:     int
////////////////////////////////////////////////////////////////////////

int CTPTradeLoginHandler::LoginFunction(IRawAPI* rawAPI, CThostFtdcReqUserLoginField* loginInfo, uint requestId)
{
	std::string value;

	if (loginInfo->BrokerID[0] == 0)
	{
		SysParam::TryGet(CTP_TRADER_BROKERID, value);
		std::strcpy(loginInfo->BrokerID, value.data());
	}

	if (loginInfo->UserID[0] == 0)
	{
		SysParam::TryGet(CTP_TRADER_USERID, value);
		std::strcpy(loginInfo->UserID, value.data());
	}
	if (loginInfo->Password[0] == 0)
	{
		SysParam::TryGet(CTP_TRADER_PASSWORD, value);
		std::strcpy(loginInfo->Password, value.data());
	}

	return ((CTPRawAPI*)rawAPI)->TrdAPI->ReqUserLogin(loginInfo, requestId);
}