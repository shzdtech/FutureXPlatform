/***********************************************************************
 * Module:  PBPricingDataSerializer.cpp
 * Author:  milk
 * Modified: 2015年8月21日 12:08:11
 * Purpose: Implementation of the class PBPricingDataSerializer
 ***********************************************************************/

#include "PBPricingDataSerializer.h"
#include "../pbserializer/PBStringTableSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/TemplateDO.h"
#include "../Protos/businessobj.pb.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       PBPricingDataSerializer::Serialize(const dataobj_ptr& abstractDO)
 // Purpose:    Implementation of PBPricingDataSerializer::Serialize()
 // Parameters:
 // - abstractDO
 // Return:     data_buffer
 ////////////////////////////////////////////////////////////////////////

data_buffer PBPricingDataSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;

	PBPricingData PB;
	auto pDO = (PricingDO*)abstractDO.get();
	FillPBHeader(PB, pDO);

	PB.set_exchange(pDO->ExchangeID());
	PB.set_contract(pDO->InstrumentID());
	double price = pDO->Bid().Price;
	if (price) PB.set_bidprice(price);

	price = pDO->Ask().Price;
	if (price) PB.set_askprice(price);

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBPricingDataSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBPricingDataSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBPricingDataSerializer::Deserialize(const data_buffer& rawdata)
{
	using namespace Micro::Future::Message::Business;
	PBPricingData PB;
	ParseWithReturn(PB, rawdata);

	auto ret = std::make_shared<PricingDO>(PB.exchange(), PB.contract());
	FillDOHeader(ret, PB);

	ret->Bid().Price = PB.bidprice();
	ret->Bid().Volume = PB.bidsize();

	ret->Ask().Price = PB.askprice();
	ret->Ask().Volume = PB.asksize();

	return ret;
}