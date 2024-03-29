/***********************************************************************
 * Module:  PBUserPositionSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:11:34
 * Purpose: Implementation of the class PBUserPositionSerializer
 ***********************************************************************/

#include "PBUserPositionSerializer.h"
#include "../pbserializer/PBStringMapSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/UserPositionDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBUserPositionSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBUserPositionSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBUserPositionSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;
	PBPosition PB;
	auto pDO = (UserPositionExDO*)abstractDO.get();
	FillPBHeader(PB, pDO);

	PB.set_exchange(pDO->ExchangeID().data());
	PB.set_contract(pDO->InstrumentID().data());
	PB.set_portfolio(pDO->PortfolioID().data());
	PB.set_direction(pDO->Direction);
	PB.set_hedgeflag(pDO->HedgeFlag);
	PB.set_positiondateflag(pDO->PositionDateFlag);
	PB.set_ydposition(pDO->LastPosition());
	PB.set_tdposition(pDO->TdPosition);
	PB.set_openvolume(pDO->OpenVolume);
	PB.set_closevolume(pDO->CloseVolume);
	PB.set_openamount(pDO->OpenAmount);
	PB.set_closeamount(pDO->CloseAmount);
	PB.set_tdcost(pDO->TdCost);
	PB.set_ydcost(pDO->YdCost);
	PB.set_usemargin(pDO->UseMargin);
	PB.set_closeprofit(pDO->CloseProfit);
	PB.set_profit(pDO->Profit());
	PB.set_opencost(pDO->OpenCost);
	PB.set_longfrozenvolume(pDO->LongFrozenVolume);
	PB.set_shortfrozenvolume(pDO->ShortFrozenVolume);
	PB.set_longfrozenamount(pDO->LongFrozenAmount);
	PB.set_shortfrozenamount(pDO->ShortFrozenAmount);
	PB.set_avgprice(pDO->AvgPrice());
	PB.set_lastprice(pDO->LastPrice);
	/*PB.set_premargin(pDO->PreMargin);
	PB.set_frozenmargin(pDO->FrozenMargin);
	PB.set_frozencash(pDO->FrozenCash);
	PB.set_frozencommission(pDO->FrozenCommission);
	PB.set_cashin(pDO->CashIn);
	PB.set_commission(pDO->Commission);
	PB.set_presettlementprice(pDO->PreSettlementPrice);
	PB.set_settlementprice(pDO->SettlementPrice);
	PB.set_tradingday(pDO->TradingDay);
	PB.set_settlementid(pDO->SettlementID);
	PB.set_exchangemargin(pDO->ExchangeMargin);
	PB.set_combposition(pDO->CombPosition);
	PB.set_comblongfrozen(pDO->CombLongFrozen);
	PB.set_combshortfrozen(pDO->CombShortFrozen);
	PB.set_closeprofitbydate(pDO->CloseProfitByDate);
	PB.set_closeprofitbytrade(pDO->CloseProfitByTrade);
	PB.set_todayposition(pDO->TodayPosition);
	PB.set_marginratebymoney(pDO->MarginRateByMoney);
	PB.set_marginratebyvolume(pDO->MarginRateByVolume);*/

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBUserPositionSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBUserPositionSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBUserPositionSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBStringMapSerializer::Instance()->Deserialize(rawdata);
}