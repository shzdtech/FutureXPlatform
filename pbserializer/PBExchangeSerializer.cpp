/***********************************************************************
 * Module:  PBExchangeSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:11:24
 * Purpose: Implementation of the class PBExchangeSerializer
 ***********************************************************************/

#include "PBExchangeSerializer.h"
#include "PBStringMapSerializer.h"
#include "pbmacros.h"
#include "proto/businessobj.pb.h"
#include "../dataobject/ExchangeDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBExchangeSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBExchangeSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBExchangeSerializer::Serialize(const dataobj_ptr abstractDO)
{
	using namespace Micro::Future::Message::Business;

	PBMarketInfo PB;
	auto pDO = (ExchangeDO*)abstractDO.get();
	FillPBHeader(PB, pDO)

	PB.set_exchange(pDO->ExchangeID);
	PB.set_name(pDO->Name);
	PB.set_property(pDO->Property);

	int bufSz = PB.ByteSize();
	uint8_t* buff = new uint8_t[bufSz];
	PB.SerializePartialToArray(buff, bufSz);
	return data_buffer(buff, bufSz);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBExchangeSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBExchangeSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBExchangeSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBStringMapSerializer::Instance()->Deserialize(rawdata);
}