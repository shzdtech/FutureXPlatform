/***********************************************************************
 * Module:  PBExchangeSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:11:24
 * Purpose: Implementation of the class PBExchangeSerializer
 ***********************************************************************/

#include "PBExchangeSerializer.h"
#include "PBStringTableSerializer.h"
#include "proto/PBMsgTrader.pb.h"
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
	PBMsgTrader::PBMsgQueryRspMarketInfo PB;
	auto pDO = (ExchangeDO*)abstractDO.get();

	PB.set_eof(pDO->EOFFlag);
	PB.set_exchangeid(pDO->ExchangeID);
	PB.set_exchangename(pDO->Name);
	PB.set_exchangeproperty(pDO->Property);

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
	return PBStringTableSerializer::Instance()->Deserialize(rawdata);
}