/***********************************************************************
 * Module:  PBMarketDataSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月10日 23:34:22
 * Purpose: Implementation of the class PBMarketDataSerializer
 ***********************************************************************/

#include "PBMarketDataSerializer.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/MarketDataDO.h"
#include "proto/businessobj.pb.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBMarketDataSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBMarketDataSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBMarketDataSerializer::Serialize(const dataobj_ptr abstractDO)
{
	Micro::Future::Message::Business::PBMarketDataList PBList;
	auto pVecDO = (VectorDO<MarketDataDO>*)abstractDO.get();

	for (auto& pDO : *pVecDO)
	{
		auto PB = PBList.add_mdlist();
		PB->set_exhange(pDO.ExchangeID());
		PB->set_contract(pDO.InstrumentID());
		PB->set_preclosevalue(pDO.PreClosePrice);
		PB->set_openvalue(pDO.OpenPrice);
		PB->set_highvalue(pDO.HighestPrice);
		PB->set_lowvalue(pDO.LowestPrice);
		PB->set_matchprice(pDO.LastPrice);
		PB->set_volume(pDO.Volume);
		PB->set_turnover(pDO.Turnover);
		//PB->set_pricechange(0);//TBD
		PB->add_askprice(pDO.AskPrice);
		PB->add_askvolume(pDO.AskVolume);
		PB->add_bidprice(pDO.BidPrice);
		PB->add_bidvolume(pDO.BidVolume);
		PB->set_highlimit(pDO.UpperLimitPrice);
		PB->set_lowlimit(pDO.LowerLimitPrice);
		PB->set_presettleprice(pDO.PreSettlementPrice);
		PB->set_settleprice(pDO.SettlementPrice);
		PB->set_preopeninterest(pDO.PreOpenInterest);
		PB->set_openinterest(pDO.OpenInterest);
		PB->set_averageprice(pDO.AveragePrice);
	}

	int bufSz = PBList.ByteSize();
	uint8_t* buff = new uint8_t[bufSz];
	PBList.SerializePartialToArray(buff, bufSz);
	return data_buffer(buff, bufSz);
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
	return nullptr;
}