/***********************************************************************
 * Module:  PBOrderSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:10:55
 * Purpose: Implementation of the class PBOrderSerializer
 ***********************************************************************/

#include "PBOrderSerializer.h"
#include "../pbserializer/PBStringTableSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/OrderDO.h"
#include "../message/BizError.h"

using namespace Micro::Future::Message::Business;

////////////////////////////////////////////////////////////////////////
// Name:       PBOrderSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBOrderSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBOrderSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	PBOrderInfo PB;
	auto pDO = (OrderDO*)abstractDO.get();
	FillPBHeader(PB, pDO);

	PB.set_brokerid(pDO->BrokerID);
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
	PB.set_volumeremain(pDO->VolumeRemain());
	PB.set_inserttime(pDO->InsertTime);
	PB.set_updatetime(pDO->UpdateTime);
	PB.set_canceltime(pDO->CancelTime);
	PB.set_tradingday(pDO->TradingDay);
	PB.set_openclose(pDO->OpenClose);
	PB.set_message(pDO->Message);
	PB.set_sessionid(pDO->SessionID);
	PB.set_portfolio(pDO->PortfolioID());
	PB.set_insertdate(pDO->InsertDate);
	PB.set_tif(pDO->TIF);
	PB.set_volumecondition(pDO->VolCondition);
	
	SerializeWithReturn(PB);
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
	PBOrderRequest PB;
	ParseWithReturn(PB, rawdata);

	auto ret = std::make_shared<OrderRequestDO>(PB.orderid(), PB.exchange(), PB.contract(), "", PB.portfolio());
	FillDOHeader(ret, PB);

	ret->OrderSysID = PB.ordersysid();
	ret->Direction = PB.direction() == DirectionType::SELL ? DirectionType::SELL : DirectionType::BUY;
	ret->LimitPrice = PB.limitprice();
	ret->Volume = PB.volume();
	ret->TIF = (OrderTIFType)PB.tif();
	ret->VolCondition = (OrderVolType)PB.volcond();
	ret->ExecType = (OrderExecType)PB.exectype();
	ret->OpenClose = (OrderOpenCloseType)PB.openclose();

	return ret;
}