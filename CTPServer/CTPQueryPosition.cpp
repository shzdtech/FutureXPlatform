/***********************************************************************
 * Module:  CTPQueryPosition.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:38:08
 * Purpose: Implementation of the class CTPQueryPosition
 ***********************************************************************/

#include "CTPQueryPosition.h"
#include "CTPRawAPI.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include <glog/logging.h>

#include "../dataobject/UserPositionDO.h"
#include "CTPUtility.h"
#include "CTPConstant.h"
////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryPosition::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryPosition::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPosition::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();
	auto& data = stdo->Data;

	auto& brokeid = session->getUserInfo()->getBrokerId();
	auto& userid = session->getUserInfo()->getInvestorId();
	auto& instrumentid = TUtil::FirstNamedEntry(STR_INSTRUMENT_ID, data, EMPTY_STRING);

	CThostFtdcQryInvestorPositionField req;
	std::memset(&req, 0, sizeof(req));
	std::strcpy(req.BrokerID, brokeid.data());
	std::strcpy(req.InvestorID, userid.data());
	std::strcpy(req.InstrumentID, instrumentid.data());
	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryInvestorPosition(&req, ++_requestIdGen);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryPosition::HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryPosition::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPosition::HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);
	dataobj_ptr ret;

	if (auto pData = (CThostFtdcInvestorPositionField*)rawRespParams[0])
	{
		auto pDO = new UserPositionExDO(EXCHANGE_CTP, pData->InstrumentID);
		ret.reset(pDO);

		pDO->Direction = (PositionDirectionType)(pData->PosiDirection - THOST_FTDC_PD_Net);
		pDO->HedgeFlag = (HedgeType)(pData->HedgeFlag - THOST_FTDC_HF_Speculation);
		pDO->PositionDate = pData->PositionDate;
		pDO->LastdayPosition = pData->YdPosition;
		pDO->Position = pData->Position;
		pDO->LongFrozen = pData->LongFrozen;
		pDO->ShortFrozen = pData->ShortFrozen;
		pDO->LongFrozenAmount = pData->LongFrozenAmount;
		pDO->ShortFrozenAmount = pData->ShortFrozenAmount;
		pDO->OpenVolume = pData->OpenVolume;
		pDO->CloseVolume = pData->CloseVolume;
		pDO->OpenAmount = pData->OpenAmount;
		pDO->CloseAmount = pData->CloseAmount;
		pDO->Cost = pData->PositionCost;
		pDO->PreMargin = pData->PreMargin;
		pDO->UseMargin = pData->UseMargin;
		pDO->FrozenMargin = pData->FrozenMargin;
		pDO->FrozenCash = pData->FrozenCash;
		pDO->FrozenCommission = pData->FrozenCommission;
		pDO->CashIn = pData->CashIn;
		pDO->Commission = pData->Commission;
		pDO->CloseProfit = pData->CloseProfit;
		pDO->Profit = pData->PositionProfit;
		pDO->PreSettlementPrice = pData->PreSettlementPrice;
		pDO->SettlementPrice = pData->SettlementPrice;
		pDO->TradingDay = pData->TradingDay;
		pDO->SettlementID = pData->SettlementID;
		pDO->OpenCost = pData->OpenCost;
		pDO->ExchangeMargin = pData->ExchangeMargin;
		pDO->CombPosition = pData->CombPosition;
		pDO->CombLongFrozen = pData->CombLongFrozen;
		pDO->CombShortFrozen = pData->CombShortFrozen;
		pDO->CloseProfitByDate = pData->CloseProfitByDate;
		pDO->CloseProfitByTrade = pData->CloseProfitByTrade;
		//pDO->TodayPosition = pData->TodayPosition;
		pDO->MarginRateByMoney = pData->MarginRateByMoney;
		pDO->MarginRateByVolume = pData->MarginRateByVolume;
	}

	return ret;
}