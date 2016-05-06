/***********************************************************************
 * Module:  PBOrderSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:10:55
 * Purpose: Implementation of the class PBOrderSerializer
 ***********************************************************************/

#include "PBOrderSerializer.h"
#include "PBStringTableSerializer.h"
#include "proto/businessobj.pb.h"
#include "../dataobject/OrderDO.h"
#include "../message/BizError.h"
#include "ExceptionDef.h"

using namespace Micro::Future::Message::Business;

////////////////////////////////////////////////////////////////////////
// Name:       PBOrderSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBOrderSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBOrderSerializer::Serialize(const dataobj_ptr abstractDO)
{
	PBOrderInfo PB;
	auto pDO = (OrderDO*)abstractDO.get();

	PB.set_orderid(pDO->OrderID);
	PB.set_ordersysid(pDO->OrderSysID);
	PB.set_exchange(pDO->ExchangeID());
	PB.set_contract(pDO->InstrumentID());
	PB.set_direction(pDO->Direction);
	PB.set_limitprice(pDO->LimitPrice);
	PB.set_volume(pDO->Volume);
	PB.set_stopprice(pDO->StopPrice);
	PB.set_active(pDO->Active);
	PB.set_orderstatus(pDO->OrderStatus);
	PB.set_volumetraded(pDO->VolumeTraded);
	PB.set_volumeremain(pDO->VolumeRemain);
	PB.set_inserttime(pDO->InsertTime);
	PB.set_updatetime(pDO->UpdateTime);
	PB.set_canceltime(pDO->CancelTime);
	PB.set_tradingday(pDO->TradingDay);
	PB.set_openclose(pDO->OpenClose);
	PB.set_message(pDO->Message);
	

	int bufSz = PB.ByteSize();
	uint8_t* buff = new uint8_t[bufSz];
	PB.SerializePartialToArray(buff, bufSz);
	return data_buffer(buff, bufSz);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBOrderSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBOrderSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBOrderSerializer::Deserialize(const data_buffer& rawdata)
{
	Micro::Future::Message::Business::PBOrderInfo PB;

	if (!PB.ParseFromArray(rawdata.get(), rawdata.size()))
		throw BizError(INVALID_DATAFORMAT_CODE, INVALID_DATAFORMAT_DESC);

	auto ret = std::make_shared<OrderDO>(PB.orderid(), PB.exchange(), PB.contract(), "");
	ret->OrderSysID = PB.ordersysid();
	ret->Direction = (DirectionType)PB.direction();
	ret->LimitPrice = PB.limitprice();
	ret->Volume = PB.volume();
	ret->StopPrice = PB.stopprice();
	ret->TIF = (OrderTIFType)PB.tif();
	ret->TradingType = (TradingType)PB.tradingtype();
	ret->ExecType = (OrderExecType)PB.exectype();
	ret->OpenClose = (OrderOpenCloseType)PB.openclose();

	return ret;
}