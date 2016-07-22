/***********************************************************************
 * Module:  CTPOTCDepthMarketData.cpp
 * Author:  milk
 * Modified: 2015年8月5日 0:05:48
 * Purpose: Implementation of the class CTPOTCDepthMarketData
 ***********************************************************************/

#include "CTPOTCDepthMarketData.h"

#include "../common/Attribute_Key.h"

#include "../CTPServer/tradeapi/ThostFtdcMdApi.h"

#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractDO.h"

#include "../message/BizError.h"
#include "../message/AppContext.h"

#include "../utility/Encoding.h"
#include "../pricingengine/AlgorithmManager.h"

#include "../common/Attribute_Key.h"
#include "../message/message_macro.h"
#include "../pricingengine/IPricingDataContext.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCDepthMarketData::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, void* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPOTCDepthMarketData::HandleResponse(const uint32_t serialId, )
 // Parameters:
 // - rawRespParams
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCDepthMarketData::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, void* rawAPI, ISession* session)
{
	dataobj_ptr ret;

	auto pData = (CThostFtdcDepthMarketDataField*)rawRespParams[0];

	if (!_mdDOMap)
		_mdDOMap = AttribPointerCast(session->getProcessor(),
			STR_KEY_SERVER_PRICING_DATACONTEXT, IPricingDataContext)
		->GetMarketDataDOMap();

	MarketDataDO& mdo = _mdDOMap->at(pData->InstrumentID);

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
	mdo.PreOpenInterest = pData->PreOpenInterest;
	mdo.OpenInterest = pData->OpenInterest;
	mdo.AveragePrice = pData->AveragePrice;

	return ret;
}