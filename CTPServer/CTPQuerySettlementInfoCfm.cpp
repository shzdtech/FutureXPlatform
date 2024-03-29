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
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"
#include "CTPUtility.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQuerySettlementInfoCfm::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQuerySettlementInfoCfm::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQuerySettlementInfoCfm::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	CTPUtility::CheckTradeInit((CTPRawAPI*)rawAPI);

	auto stdo = (StringMapDO<std::string>*)reqDO.get();

	auto& brokeid = session->getUserInfo().getBrokerId();
	auto& investorid = session->getUserInfo().getInvestorId();
	auto& cfmdate = stdo->TryFind(STR_DATE, EMPTY_STRING);
	auto& cfmtime = stdo->TryFind(STR_TIME, EMPTY_STRING);

	CThostFtdcSettlementInfoConfirmField req{};
	std::strncpy(req.BrokerID, brokeid.data(), sizeof(req.BrokerID));
	std::strncpy(req.InvestorID, investorid.data(), sizeof(req.InvestorID));
	std::strncpy(req.ConfirmDate, cfmdate.data(), sizeof(req.ConfirmDate));
	std::strncpy(req.ConfirmTime, cfmtime.data(), sizeof(req.ConfirmTime));

	int iRet = ((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqSettlementInfoConfirm(&req, serialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQuerySettlementInfoCfm::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQuerySettlementInfoCfm::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQuerySettlementInfoCfm::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);
	
	auto pDO = new StringMapDO<std::string>();
	dataobj_ptr ret(pDO);

	if (auto pData = (CThostFtdcSettlementInfoConfirmField*)rawRespParams[0])
	{
		pDO->HasMore = !*(bool*)rawRespParams[3];
		(*pDO)[STR_BROKER_ID] = pData->BrokerID;
		(*pDO)[STR_USER_ID] = pData->InvestorID;
		(*pDO)[STR_DATE] = pData->ConfirmDate;
		(*pDO)[STR_TIME] = pData->ConfirmTime;
	}

	return ret;
}