/***********************************************************************
 * Module:  PBSubMarketDataSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月16日 22:23:16
 * Purpose: Implementation of the class PBSubMarketDataSerializer
 ***********************************************************************/

#include "PBSubMarketDataSerializer.h"
#include "../pbserializer/PBStringTableSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../Protos/businessobj.pb.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBSubMarketDataSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBSubMarketDataSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBSubMarketDataSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;

	PBMarketDataList PBList;
	auto pVecDO = (VectorDO<MarketDataDO>*)abstractDO.get();
	FillPBHeader(PBList, pVecDO);

	for (auto& pDO : *pVecDO)
	{
		auto PB = PBList.add_marketdata();
		PB->set_exchange(pDO.ExchangeID());
		PB->set_contract(pDO.InstrumentID());

		// Updating with time
		PB->set_highvalue(pDO.HighestPrice);
		PB->set_lowvalue(pDO.LowestPrice);
		PB->set_matchprice(pDO.LastPrice);
		PB->set_volume(pDO.Volume);
		PB->set_turnover(pDO.Turnover);
		PB->add_askprice(pDO.Ask().Price);
		PB->add_askvolume(pDO.Ask().Volume);
		PB->add_bidprice(pDO.Bid().Price);
		PB->add_bidvolume(pDO.Bid().Volume);
		PB->set_averageprice(pDO.AveragePrice);

		// Doesn't change after openning
		PB->set_preclosevalue(pDO.PreClosePrice);
		PB->set_openvalue(pDO.OpenPrice);
		PB->set_highlimit(pDO.UpperLimitPrice);
		PB->set_lowlimit(pDO.LowerLimitPrice);
		PB->set_presettleprice(pDO.PreSettlementPrice);
		PB->set_settleprice(pDO.SettlementPrice);
		PB->set_preopeninterest(pDO.PreOpenInterest);
		PB->set_openinterest(pDO.OpenInterest);
	}

	SerializeWithReturn(PBList);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBSubMarketDataSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBSubMarketDataSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBSubMarketDataSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBStringTableSerializer::Instance()->Deserialize(rawdata);
}