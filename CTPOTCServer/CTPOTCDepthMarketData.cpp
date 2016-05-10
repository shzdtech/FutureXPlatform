/***********************************************************************
 * Module:  CTPOTCDepthMarketData.cpp
 * Author:  milk
 * Modified: 2015年8月5日 0:05:48
 * Purpose: Implementation of the class CTPOTCDepthMarketData
 ***********************************************************************/

#include "CTPOTCDepthMarketData.h"
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/Attribute_Key.h"

#include "../CTPServer/tradeapi/ThostFtdcMdApi.h"

#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParam.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractDO.h"

#include "../message/BizError.h"
#include "../message/AppContext.h"

#include "../utility/Encoding.h"
#include <glog/logging.h>
#include "../pricingengine/AlgorithmManager.h"

#include "../pricingengine/PricingContext.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCDepthMarketData::HandleResponse(ParamVector& rawRespParams, void* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCDepthMarketData::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCDepthMarketData::HandleResponse(ParamVector& rawRespParams, void* rawAPI, ISession* session)
{
	dataobj_ptr ret;

	auto pData = (CThostFtdcDepthMarketDataField*)rawRespParams[0];

	auto pMdMap = PricingContext::Instance()->GetMarketDataDOMap();

	MarketDataDO& mdo = pMdMap->at(pData->InstrumentID);

	mdo.PreClosePrice = pData->PreClosePrice;
	mdo.OpenPrice = pData->OpenPrice;
	mdo.HighestPrice = pData->HighestPrice;
	mdo.LowestPrice = pData->LowestPrice;
	mdo.LastPrice = pData->LastPrice;
	mdo.Volume = pData->Volume;
	mdo.Turnover = pData->Turnover;
	//p.gFutMDPB->set_pricechange(0);//TBD
	mdo.BidPrice = pData->BidPrice1;
	mdo.AskPrice = pData->AskPrice1;
	mdo.BidVolume = pData->BidVolume1;
	mdo.AskVolume = pData->AskVolume1;
	mdo.UpperLimitPrice = pData->UpperLimitPrice;
	mdo.LowerLimitPrice = pData->LowerLimitPrice;
	mdo.PreSettlementPrice = pData->PreSettlementPrice;
	mdo.SettlementPrice = pData->SettlementPrice;

	return ret;
}