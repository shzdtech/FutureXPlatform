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
	PB.set_bidprice(pDO->BidPrice);
	PB.set_askprice(pDO->AskPrice);

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
	return PBStringTableSerializer::Instance()->Deserialize(rawdata);
}