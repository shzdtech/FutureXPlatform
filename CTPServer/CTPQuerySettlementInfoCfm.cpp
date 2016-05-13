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
#include <glog/logging.h>
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
	auto stdo = (StringTableDO*)reqDO.get();
	auto& data = stdo->Data;

	auto& brokeid = session->getUserInfo()->getBrokerId();
	auto& userid = session->getUserInfo()->getInvestorId();
	auto& cfmdate = TUtil::FirstNamedEntry(STR_DATE, data, EMPTY_STRING);
	auto& cfmtime = TUtil::FirstNamedEntry(STR_TIME, data, EMPTY_STRING);

	CThostFtdcSettlementInfoConfirmField req;
	std::memset(&req, 0, sizeof(req));
	std::strcpy(req.BrokerID, brokeid.data());
	std::strcpy(req.InvestorID, userid.data());
	std::strcpy(req.ConfirmDate, cfmdate.data());
	std::strcpy(req.ConfirmTime, cfmtime.data());

	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqSettlementInfoConfirm(&req, ++_requestIdGen);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQuerySettlementInfoCfm::HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQuerySettlementInfoCfm::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQuerySettlementInfoCfm::HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);

	dataobj_ptr ret;

	if (auto pData = (CThostFtdcSettlementInfoConfirmField*)rawRespParams[0])
	{
		auto pDO = new StringTableDO;
		ret.reset(pDO);

		pDO->Data[STR_BROKER_ID].push_back(pData->BrokerID);
		pDO->Data[STR_USER_ID].push_back(pData->InvestorID);
		pDO->Data[STR_DATE].push_back(pData->ConfirmDate);
		pDO->Data[STR_TIME].push_back(pData->ConfirmTime);

		DLOG(INFO) << "Settlement Info Confirm" << std::endl;
	}

	return ret;
}