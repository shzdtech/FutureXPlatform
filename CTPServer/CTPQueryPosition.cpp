/***********************************************************************
 * Module:  CTPQueryPosition.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:38:08
 * Purpose: Implementation of the class CTPQueryPosition
 ***********************************************************************/

#include "CTPQueryPosition.h"
#include "CTPRawAPI.h"
#include "CTPUtility.h"
#include "CTPConstant.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../common/BizErrorIDs.h"
#include "../common/Attribute_Key.h"
#include "../dataobject/UserPositionDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"
#include "../bizutility/ContractCache.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryPosition::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPQueryPosition::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPosition::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& brokeid = session->getUserInfo()->getBrokerId();
	auto& investorid = session->getUserInfo()->getInvestorId();
	auto& userid = session->getUserInfo()->getUserId();
	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);

	CThostFtdcQryInvestorPositionField req{};
	std::strncpy(req.BrokerID, brokeid.data(), sizeof(req.BrokerID) - 1);
	std::strncpy(req.InvestorID, investorid.data(), sizeof(req.InvestorID) - 1);
	std::strncpy(req.InstrumentID, instrumentid.data(), sizeof(req.InstrumentID) - 1);

	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryInvestorPosition(&req, serialId);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(session->getProcessor()))
	{
		auto positionMap = pWorkerProc->GetUserPositionContext().GetPositionsByUser(userid);
		if (positionMap.empty())
		{
			std::this_thread::sleep_for(CTPProcessor::DefaultQueryTime);
			positionMap = pWorkerProc->GetUserPositionContext().GetPositionsByUser(userid);
		}

		if (positionMap.empty())
		{
			throw NotFoundException();
		}

		if (instrumentid != EMPTY_STRING)
		{
			UserPositionExDO_Ptr userPositionPtr;
			if (!positionMap.map()->find(instrumentid, userPositionPtr))
				throw NotFoundException();

			auto position_ptr = std::make_shared<UserPositionExDO>(*userPositionPtr);
			position_ptr->HasMore = false;

			pWorkerProc->SendDataObject(session, MSG_ID_QUERY_POSITION, serialId, position_ptr);
		}
		else
		{
			auto locktb = positionMap.map()->lock_table();
			auto endit = locktb.end();
			for (auto it = locktb.begin(); it != endit; it++)
			{
				auto positionDO_Ptr = std::make_shared<UserPositionExDO>(*it->second);
				positionDO_Ptr->HasMore = std::next(it) != endit;

				pWorkerProc->SendDataObject(session, MSG_ID_QUERY_POSITION, serialId, positionDO_Ptr);
			}
		}
	}
	else
	{
		CTPUtility::CheckReturnError(iRet);
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryPosition::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryPosition::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPosition::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);
	UserPositionExDO_Ptr ret;
	auto pData = (CThostFtdcInvestorPositionField*)rawRespParams[0];

	std::string exchange;
	if (auto pInstrumentDO = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentById(pData->InstrumentID))
	{
		exchange = pInstrumentDO->ExchangeID();
	}

	auto pDO = new UserPositionExDO(exchange, pData->InstrumentID);
	ret.reset(pDO);

	pDO->HasMore = !*(bool*)rawRespParams[3];

	pDO->Direction = (PositionDirectionType)(pData->PosiDirection - THOST_FTDC_PD_Net);
	pDO->HedgeFlag = (HedgeType)(pData->HedgeFlag - THOST_FTDC_HF_Speculation);
	pDO->PositionDateFlag = (PositionDateFlagType)(pData->PositionDate - THOST_FTDC_PSD_Today);
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

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(session->getProcessor()))
	{
		if (pData->PositionDate == THOST_FTDC_PSD_Today)
		{
			pWorkerProc->GetUserPositionContext().UpsertPosition(session->getUserInfo()->getUserId(), ret);
		}
		else
		{
			auto position_ptr = pWorkerProc->GetUserPositionContext().GetPosition(session->getUserInfo()->getUserId(), pDO->InstrumentID());
			if (!position_ptr)
			{
				ret->PositionDateFlag = PositionDateFlagType::PSD_TODAY;
				pWorkerProc->GetUserPositionContext().UpsertPosition(session->getUserInfo()->getUserId(), ret);
			}
		}

		ret.reset();
	}

	return ret;
}