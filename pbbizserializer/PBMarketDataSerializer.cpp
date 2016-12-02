/***********************************************************************
 * Module:  PBMarketDataSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月10日 23:34:22
 * Purpose: Implementation of the class PBMarketDataSerializer
 ***********************************************************************/

#include "PBMarketDataSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../Protos/businessobj.pb.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBMarketDataSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBMarketDataSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBMarketDataSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;
	
	PBMarketData PB;
	auto pDO = (MarketDataDO*)abstractDO.get();
	FillPBHeader(PB, pDO);

	PB.set_exchange(pDO->ExchangeID());
	PB.set_contract(pDO->InstrumentID());

	// Updating with time
	PB.set_highvalue(pDO->HighestPrice);
	PB.set_lowvalue(pDO->LowestPrice);
	PB.set_matchprice(pDO->LastPrice);
	PB.set_volume(pDO->Volume);
	PB.set_turnover(pDO->Turnover);
	PB.add_askprice(pDO->Ask().Price);
	PB.add_askvolume(pDO->Ask().Volume);
	PB.add_bidprice(pDO->Bid().Price);
	PB.add_bidvolume(pDO->Bid().Volume);
	PB.set_averageprice(pDO->AveragePrice);

	if (pDO->UpdateTime)
		PB.set_updatetime(pDO->UpdateTime);

	// Doesn't change after openning
	if(pDO->TradingDay > 0)
		PB.set_tradingday(pDO->TradingDay);
	PB.set_preclosevalue(pDO->PreClosePrice);
	PB.set_openvalue(pDO->OpenPrice);
	PB.set_highlimit(pDO->UpperLimitPrice);
	PB.set_lowlimit(pDO->LowerLimitPrice);
	PB.set_presettleprice(pDO->PreSettlementPrice);
	PB.set_settleprice(pDO->SettlementPrice);
	PB.set_preopeninterest(pDO->PreOpenInterest);
	PB.set_openinterest(pDO->OpenInterest);

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBMarketDataSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBMarketDataSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBMarketDataSerializer::Deserialize(const data_buffer& rawdata)
{
	using namespace Micro::Future::Message::Business;
	PBMarketData PB;
	ParseWithReturn(PB, rawdata);

	auto ret = std::make_shared<MarketDataDO>(PB.exchange(), PB.contract());
	FillDOHeader(ret, PB);

	ret->Bid().Price = PB.bidprice().Get(0);
	ret->Bid().Volume = PB.bidvolume().Get(0);

	ret->Ask().Price = PB.askprice().Get(0);
	ret->Ask().Volume = PB.askvolume().Get(0);

	return ret;
}