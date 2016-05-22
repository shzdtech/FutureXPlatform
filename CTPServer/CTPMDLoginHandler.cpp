/***********************************************************************
 * Module:  CTPMDLoginHandler.cpp
 * Author:  milk
 * Modified: 2015年3月8日 13:12:15
 * Purpose: Implementation of the class CTPMDLoginHandler
 ***********************************************************************/

#include "CTPMDLoginHandler.h"
#include "CTPRawAPI.h"
#include "CTPConstant.h"
#include "../utility/TUtil.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPMDLoginHandler::LoginFunction()
// Purpose:    Implementation of CTPMDLoginHandler::LoginFunction()
// Return:     int
////////////////////////////////////////////////////////////////////////

int CTPMDLoginHandler::LoginFunction(IRawAPI* rawAPI, CThostFtdcReqUserLoginField* loginInfo, uint32_t requestId)
{
	std::string value;

	if (loginInfo->BrokerID[0] == 0)
	{
		SysParam::TryGet(CTP_MD_BROKERID, value);
		std::strcpy(loginInfo->BrokerID, value.data());
	}
	if (loginInfo->UserID[0] == 0)
	{
		SysParam::TryGet(CTP_MD_USERID, value);
		std::strcpy(loginInfo->UserID, value.data());
	}
	if (loginInfo->Password[0] == 0)
	{
		SysParam::TryGet(CTP_MD_PASSWORD, value);
		std::strcpy(loginInfo->Password, value.data());
	}

	return ((CTPRawAPI*)rawAPI)->MdAPI->ReqUserLogin(loginInfo, requestId);
}