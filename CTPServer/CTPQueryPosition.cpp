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
#include "../message/GlobalProcessorRegistry.h"
#include "../common/BizErrorIDs.h"
#include "../common/Attribute_Key.h"
#include "../dataobject/UserPositionDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include "../bizutility/InstrumentCache.h"


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
	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& brokeid = session->getUserInfo()->getBrokerId();
	auto& userid = session->getUserInfo()->getInvestorId();
	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
	CThostFtdcQryInvestorPositionField req{};
	std::strcpy(req.BrokerID, brokeid.data());
	std::strcpy(req.InvestorID, userid.data());
	std::strcpy(req.InstrumentID, instrumentid.data());
	
	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryInvestorPosition(&req, reqDO->SerialId);
	CTPUtility::CheckReturnError(iRet);

	if (auto wkProcPtr = std::static_pointer_cast<CTPTradeWorkerProcessor>
		(GlobalProcessorRegistry::FindProcessor(CTPWorkerProcessorID::TRADE_SHARED_ACCOUNT)))
	{
		auto& positionMap = wkProcPtr->GetUserPositionMap();
		if (positionMap.size() < 1)
		{
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}

		if (instrumentid != EMPTY_STRING)
		{
			auto it = positionMap.find(instrumentid);
			if (it != positionMap.end())
			{
				auto& positions = it->second;
				auto lastpit = std::prev(positions.end());
				for (auto pit = positions.begin(); pit != positions.end(); pit++)
				{
					auto position_ptr = std::make_shared<UserPositionExDO>(pit->second);
					position_ptr->SerialId = reqDO->SerialId;
					if (pit == lastpit)
						position_ptr->HasMore = true;
					wkProcPtr->SendDataObject(session, MSG_ID_QUERY_POSITION, position_ptr);
				}
			}
		}
		else
		{
			auto lastit = std::prev(positionMap.end());
			for (auto it = positionMap.begin(); it != positionMap.end(); it++)
			{
				auto& positions = it->second;
				auto lastpit = std::prev(positions.end());
				for (auto pit = positions.begin(); pit != positions.end(); pit++)
				{
					auto positionDO_Ptr = std::make_shared<UserPositionExDO>(pit->second);
					positionDO_Ptr->SerialId = reqDO->SerialId;
					if (it == lastit && pit == lastpit)
						positionDO_Ptr->HasMore = true;
					wkProcPtr->SendDataObject(session, MSG_ID_QUERY_POSITION, positionDO_Ptr);
				}
			}
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryPosition::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryPosition::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPosition::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);
	dataobj_ptr ret;

	if (auto pData = (CThostFtdcInvestorPositionField*)rawRespParams[0])
	{
		std::string exchange;

		if (auto pInstrumentDO = InstrumentCache::QueryInstrumentById(pData->InstrumentID))
		{
			exchange = pInstrumentDO->ExchangeID();
		}
		auto pDO = new UserPositionExDO(exchange, pData->InstrumentID);
		ret.reset(pDO);
		pDO->SerialId = serialId;
		pDO->HasMore = !*(bool*)rawRespParams[3];

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

		if (auto wkProcPtr = std::static_pointer_cast<CTPTradeWorkerProcessor>
			(GlobalProcessorRegistry::FindProcessor(CTPWorkerProcessorID::TRADE_SHARED_ACCOUNT)))
		{
			auto& positionMap = wkProcPtr->GetUserPositionMap();
			auto& positions = positionMap.getorfill(pDO->InstrumentID());
			auto& position = positions.getorfill(pDO->Direction);
			position = *pDO;
		}
	}

	return ret;
}