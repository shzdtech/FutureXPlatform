/***********************************************************************
 * Module:  CTPDepthMarketData.cpp
 * Author:  milk
 * Modified: 2015年3月6日 20:17:29
 * Purpose: Implementation of the class CTPDepthMarketData
 ***********************************************************************/

#include "CTPDepthMarketData.h"
#include "tradeapi/ThostFtdcMdApi.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/TemplateDO.h"
#include "../message/BizError.h"
#include <boost/locale/encoding.hpp>

#include "CTPUtility.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPDepthMarketData::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPDepthMarketData::HandleResponse(const uint32_t serialId, )
 // Parameters:
 // - rawRespParams
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPDepthMarketData::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto pData = (CThostFtdcDepthMarketDataField*)rawRespParams[0];
	MarketDataDO* mdo = new MarketDataDO(pData->ExchangeID, pData->InstrumentID);
	dataobj_ptr ret(mdo);

	mdo->PreClosePrice = pData->PreClosePrice;
	mdo->UpperLimitPrice = pData->UpperLimitPrice;
	mdo->LowerLimitPrice = pData->LowerLimitPrice;
	mdo->PreSettlementPrice = pData->PreSettlementPrice;

	mdo->PreOpenInterest = pData->PreOpenInterest;
	mdo->OpenInterest = pData->OpenInterest;
	mdo->LastPrice = pData->LastPrice;
	mdo->Volume = pData->Volume;

	if (pData->OpenPrice < 1e32)
	{
		mdo->OpenPrice = pData->OpenPrice;
		mdo->SettlementPrice = pData->SettlementPrice;

		mdo->HighestPrice = pData->HighestPrice;
		mdo->LowestPrice = pData->LowestPrice;
		mdo->Turnover = pData->Turnover;
		mdo->AveragePrice = pData->AveragePrice;
	}

	if (pData->BidPrice1 < 1e32)
	{
		mdo->Bid().Price = pData->BidPrice1;
		mdo->Bid().Volume = pData->BidVolume1;
	}

	if (pData->AskPrice1 < 1e32)
	{
		mdo->Ask().Price = pData->AskPrice1;
		mdo->Ask().Volume = pData->AskVolume1;
	}

	if (pData->UpdateTime[0])
	{
		int hour, min, sec;
		std::sscanf(pData->UpdateTime, "%d:%d:%d", &hour, &min, &sec);
		mdo->UpdateTime = hour * 3600 + min * 60 + sec;

		mdo->TradingDay = session->getUserInfo().getTradingDay();
	}

	return ret;
}