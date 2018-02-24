/***********************************************************************
 * Module:  PBPositionPnLSerializer.cpp
 * Author:  milk
 * Modified: 2018年1月12日 19:11:34
 * Purpose: Implementation of the class PBPositionPnLSerializer
 ***********************************************************************/

#include "PBPositionPnLSerializer.h"
#include "../pbserializer/PBStringMapSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/UserPositionDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBPositionPnLSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBPositionPnLSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBPositionPnLSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;
	PBPositionPnL PB;
	auto pDO = (PositionPnLDO*)abstractDO.get();
	FillPBHeader(PB, pDO);

	PB.set_exchange(pDO->ExchangeID().data());
	PB.set_contract(pDO->InstrumentID().data());
	PB.set_portfolio(pDO->PortfolioID().data());
	PB.set_avgbuyprice(pDO->BuyAvgPrice());
	PB.set_avgsellprice(pDO->SellAvgPrice());
	PB.set_buyprofit(pDO->BuyProfit());
	PB.set_sellprofit(pDO->SellProfit());
	PB.set_tdbuyamount(pDO->TdBuyAmount);
	PB.set_tdsellamount(pDO->TdSellAmount);
	PB.set_tdbuyposition(pDO->TdBuyVolume);
	PB.set_tdsellposition(pDO->TdSellVolume);
	PB.set_ydbuyposition(pDO->YdBuyVolume);
	PB.set_ydsellposition(pDO->YdSellVolume);

	PB.set_lastprice(pDO->LastPrice);

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBPositionPnLSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBPositionPnLSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBPositionPnLSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBStringMapSerializer::Instance()->Deserialize(rawdata);
}