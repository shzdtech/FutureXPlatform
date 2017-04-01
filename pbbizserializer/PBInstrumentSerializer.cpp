/***********************************************************************
 * Module:  PBInstrumentSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:10:41
 * Purpose: Implementation of the class PBInstrumentSerializer
 ***********************************************************************/

#include "PBInstrumentSerializer.h"
#include "../pbserializer/PBStringMapSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/ContractKey.h"
#include "../dataobject/TypedefDO.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       PBInstrumentSerializer::Serialize(const dataobj_ptr& abstractDO)
 // Purpose:    Implementation of PBInstrumentSerializer::Serialize()
 // Parameters:
 // - abstractDO
 // Return:     data_buffer
 ////////////////////////////////////////////////////////////////////////

using namespace Micro::Future::Message::Business;

data_buffer PBInstrumentSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	PBInstrumentList PB;
	auto pDO = (ContractList*)abstractDO.get();
	FillPBHeader(PB, pDO)

	for (auto& insDO : *pDO)
	{
		auto pInst = PB.add_instrument();
		pInst->set_exchange(insDO.ExchangeID().data());
		pInst->set_contract(insDO.InstrumentID().data());
	}

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBInstrumentSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBInstrumentSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBInstrumentSerializer::Deserialize(const data_buffer& rawdata)
{
	PBInstrumentList PB;
	ParseWithReturn(PB, rawdata);

	auto ret = std::make_shared<ContractList>();
	FillDOHeader(ret, PB);

	for (auto& instrument : PB.instrument())
	{
		ret->push_back(ContractKey(instrument.exchange(), instrument.contract()));
	}

	return ret;
}