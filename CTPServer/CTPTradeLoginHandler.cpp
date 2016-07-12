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
#include "../common/Attribute_Key.h"
#include "../dataobject/TypedefDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeLoginHandler::LoginFunction()
// Purpose:    Implementation of CTPTradeLoginHandler::LoginFunction()
// Return:     int
////////////////////////////////////////////////////////////////////////

int CTPTradeLoginHandler::LoginFunction(IRawAPI* rawAPI, ISession* session, CThostFtdcReqUserLoginField* loginInfo, uint requestId)
{
	std::string value;

	if (loginInfo->BrokerID[0] == 0)
	{
		std::strcpy(loginInfo->BrokerID, SysParam::Get(CTP_TRADER_BROKERID).data());
	}

	if (loginInfo->UserID[0] == 0)
	{
		std::strcpy(loginInfo->UserID, SysParam::Get(CTP_TRADER_USERID).data());
	}
	if (loginInfo->Password[0] == 0)
	{
		std::strcpy(loginInfo->Password, SysParam::Get(CTP_TRADER_PASSWORD).data());
	}

	return ((CTPRawAPI*)rawAPI)->TrdAPI->ReqUserLogin(loginInfo, requestId);
}

dataobj_ptr CTPTradeLoginHandler::HandleResponse(const uint32_t serialId, param_vector & rawRespParams, IRawAPI * rawAPI, ISession * session)
{
	auto ret = CTPLoginHandler::HandleResponse(serialId, rawRespParams, rawAPI, session);

	return ret;
}
