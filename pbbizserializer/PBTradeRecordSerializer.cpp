/***********************************************************************
 * Module:  PBTradeRecordSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:11:05
 * Purpose: Implementation of the class PBTradeRecordSerializer
 ***********************************************************************/

#include "PBTradeRecordSerializer.h"
#include "../pbserializer/PBStringMapSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/TradeRecordDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBTradeRecordSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBTradeRecordSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBTradeRecordSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;
	
	PBTradeInfo PB;
	auto pDO = (TradeRecordDO*)abstractDO.get();
	FillPBHeader(PB, pDO);

	PB.set_brokerid(pDO->BrokerID);
	PB.set_exchange(pDO->ExchangeID().data());
	PB.set_contract(pDO->InstrumentID().data());
	PB.set_userid(pDO->UserID().data());
	PB.set_ordersysid(pDO->OrderSysID);
	PB.set_orderid(pDO->OrderID);
	PB.set_tradeid(pDO->TradeID);
	PB.set_direction(pDO->Direction);
	PB.set_openclose(pDO->OpenClose);
	PB.set_price(pDO->Price);
	PB.set_volume(pDO->Volume);
	PB.set_tradedate(pDO->TradeDate);
	PB.set_tradetime(pDO->TradeTime);
	PB.set_tradetype(pDO->TradeType);
	PB.set_hedgeflag(pDO->HedgeFlag);
	PB.set_portfolio(pDO->PortfolioID());

	SerializeWithReturn(PB);
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
	Micro::Future::Message::Business::PBTradeInfo PB;
	ParseWithReturn(PB, rawdata);

	auto trade = std::make_shared<TradeRecordDO>(PB.exchange(), PB.contract(), "", PB.portfolio());
	FillDOHeader(trade, PB);

	trade->TradeID = PB.tradeid();
	trade->OrderID = PB.orderid();
	trade->OrderSysID = PB.ordersysid();
	trade->Direction = PB.direction() == DirectionType::SELL ? DirectionType::SELL : DirectionType::BUY;
	trade->OpenClose = (OrderOpenCloseType)PB.openclose();
	trade->Price = PB.price();
	trade->Volume = PB.volume();
	trade->HedgeFlag = (HedgeType)PB.hedgeflag();
	trade->TradeType = (OrderTradingType)PB.tradetype();
	
	return trade;
}