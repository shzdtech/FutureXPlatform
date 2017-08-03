/***********************************************************************
 * Module:  PBSubMarketDataSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月16日 22:23:16
 * Purpose: Implementation of the class PBSubMarketDataSerializer
 ***********************************************************************/

#include "PBSubPricingDataSerializer.h"
#include "PBPricingDataSerializer.h"
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

data_buffer PBSubPricingDataSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;

	PBPricingDataList PBList;
	auto pVecDO = (VectorDO<PricingDO>*)abstractDO.get();
	FillPBHeader(PBList, pVecDO);

	for (auto& pDO : *pVecDO)
	{
		auto PB = PBList.add_pricingdata();
		PB->set_exchange(pDO.ExchangeID());
		PB->set_contract(pDO.InstrumentID());
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

dataobj_ptr PBSubPricingDataSerializer::Deserialize(const data_buffer& rawdata)
{
	Micro::Future::Message::Business::PBInstrumentList PB;
	ParseWithReturn(PB, rawdata);

	auto ret = std::make_shared<VectorDO<ContractKey>>();
	FillDOHeader(ret, PB);

	for (auto& hc : PB.instrument())
	{
		ret->push_back(std::move(ContractKey(hc.exchange(), hc.contract())));
	}

	return ret;
}