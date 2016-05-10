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
#include "../utility/Encoding.h"
#include <glog/logging.h>
#include "CTPUtility.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPDepthMarketData::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPDepthMarketData::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPDepthMarketData::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto pData = (CThostFtdcDepthMarketDataField*)rawRespParams[0];
	auto pDOVec = new VectorDO<MarketDataDO>;
	dataobj_ptr ret(pDOVec);

	MarketDataDO mdo(pData->ExchangeID, pData->InstrumentID);
	mdo.PreClosePrice = pData->PreClosePrice;
	mdo.OpenPrice = pData->OpenPrice;
	mdo.HighestPrice = pData->HighestPrice;
	mdo.LowestPrice = pData->LowestPrice;
	mdo.LastPrice = pData->LastPrice;
	mdo.Volume = pData->Volume;
	mdo.Turnover = pData->Turnover;
	mdo.BidPrice = pData->BidPrice1;
	mdo.AskPrice = pData->AskPrice1;
	mdo.BidVolume = pData->BidVolume1;
	mdo.AskVolume = pData->AskVolume1;
	mdo.UpperLimitPrice = pData->UpperLimitPrice;
	mdo.LowerLimitPrice = pData->LowerLimitPrice;
	mdo.PreSettlementPrice = pData->PreSettlementPrice;
	mdo.SettlementPrice = pData->SettlementPrice;
	mdo.PreOpenInterest = pData->PreOpenInterest;
	mdo.OpenInterest = pData->OpenInterest;
	mdo.AveragePrice = pData->AveragePrice;

	pDOVec->push_back(std::move(mdo));

	return ret;
}