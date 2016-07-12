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

int CTPMDLoginHandler::LoginFunction(IRawAPI* rawAPI, ISession* session, CThostFtdcReqUserLoginField* loginInfo, uint requestId)
{
	std::string value;

	if (loginInfo->BrokerID[0] == 0)
	{
		std::strcpy(loginInfo->BrokerID, SysParam::Get(CTP_MD_BROKERID).data());
	}
	if (loginInfo->UserID[0] == 0)
	{
		std::strcpy(loginInfo->UserID, SysParam::Get(CTP_MD_USERID).data());
	}
	if (loginInfo->Password[0] == 0)
	{
		std::strcpy(loginInfo->Password, SysParam::Get(CTP_MD_PASSWORD).data());
	}

	return ((CTPMarketDataProcessor*)session->getProcessor().get())->
		Login(loginInfo, requestId);
}