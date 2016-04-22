/***********************************************************************
 * Module:  PBTradeRecordSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:11:05
 * Purpose: Implementation of the class PBTradeRecordSerializer
 ***********************************************************************/

#include "PBTradeRecordSerializer.h"
#include "PBStringTableSerializer.h"
#include "proto/businessobj.pb.h"
#include "../dataobject/TradeRecordDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBTradeRecordSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBTradeRecordSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBTradeRecordSerializer::Serialize(const dataobj_ptr abstractDO)
{
	Micro::Future::Message::Business::PBTradeNotification PB;
	auto pDO = (TradeRecordDO*)abstractDO.get();

	PB.set_exchange(pDO->ExchangeID().data());
	PB.set_contract(pDO->InstrumentID().data());
	PB.set_ordersysid(pDO->OrderSysID);
	PB.set_orderid(pDO->OrderID);
	PB.set_tradeid(pDO->TradeID);
	PB.set_direction(pDO->Direction);
	PB.set_price(pDO->Price);
	PB.set_volume(pDO->Volume);
	PB.set_tradedate(pDO->TradeDate);
	PB.set_tradetime(pDO->TradeTime);
	PB.set_tradetype(pDO->TradeType);

	int bufSz = PB.ByteSize();
	uint8_t* buff = new uint8_t[bufSz];
	PB.SerializePartialToArray(buff, bufSz);
	return data_buffer(buff, bufSz);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBTradeRecordSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBTradeRecordSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBTradeRecordSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBStringTableSerializer::Instance()->Deserialize(rawdata);
}