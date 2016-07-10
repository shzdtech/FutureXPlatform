/***********************************************************************
 * Module:  CTPQuerySettlementInfoCfm.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:38:54
 * Purpose: Implementation of the class CTPQuerySettlementInfoCfm
 ***********************************************************************/

#include "CTPQuerySettlementInfoCfm.h"
#include "CTPRawAPI.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include "CTPUtility.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQuerySettlementInfoCfm::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPQuerySettlementInfoCfm::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQuerySettlementInfoCfm::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& brokeid = session->getUserInfo()->getBrokerId();
	auto& userid = session->getUserInfo()->getInvestorId();
	auto& cfmdate = stdo->TryFind(STR_DATE, EMPTY_STRING);
	auto& cfmtime = stdo->TryFind(STR_TIME, EMPTY_STRING);

	CThostFtdcSettlementInfoConfirmField req{};
	std::strcpy(req.BrokerID, brokeid.data());
	std::strcpy(req.InvestorID, userid.data());
	std::strcpy(req.ConfirmDate, cfmdate.data());
	std::strcpy(req.ConfirmTime, cfmtime.data());

	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqSettlementInfoConfirm(&req, reqDO->SerialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQuerySettlementInfoCfm::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQuerySettlementInfoCfm::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQuerySettlementInfoCfm::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);
	
	auto pDO = new MapDO<std::string>();
	dataobj_ptr ret(pDO);

	if (auto pData = (CThostFtdcSettlementInfoConfirmField*)rawRespParams[0])
	{
		pDO->SerialId = serialId;
		pDO->HasMore = !*(bool*)rawRespParams[3];
		(*pDO)[STR_BROKER_ID] = pData->BrokerID;
		(*pDO)[STR_USER_ID] = pData->InvestorID;
		(*pDO)[STR_DATE] = pData->ConfirmDate;
		(*pDO)[STR_TIME] = pData->ConfirmTime;
	}

	return ret;
}