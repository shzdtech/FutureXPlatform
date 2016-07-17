/***********************************************************************
 * Module:  PBAccountInfoSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:11:15
 * Purpose: Implementation of the class PBAccountInfoSerializer
 ***********************************************************************/

#include "PBAccountInfoSerializer.h"
#include "../pbserializer/PBStringMapSerializer.h"
#include "../Protos/businessobj.pb.h"
#include "../pbserializer/pbmacros.h"
#include "../dataobject/AccountInfoDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBAccountInfoSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBAccountInfoSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBAccountInfoSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;
	
	PBAccountInfo PB;
	auto pDO = (AccountInfoDO*)abstractDO.get();

	FillPBHeader(PB, pDO);

	PB.set_brokerid(pDO->BrokerID);
	PB.set_accountid(pDO->AccountID);
	PB.set_premortgage(pDO->PreMortgage);
	PB.set_precredit(pDO->PreCredit);
	PB.set_predeposit(pDO->PreDeposit);

	PB.set_prebalance(pDO->PreBalance);
	PB.set_premargin(pDO->PreMargin);
	PB.set_interestbase(pDO->InterestBase);
	PB.set_interest(pDO->Interest);
	PB.set_deposit(pDO->Deposit);
	PB.set_withdraw(pDO->Withdraw);
	PB.set_frozenmargin(pDO->FrozenMargin);
	PB.set_frozencash(pDO->FrozenCash);
	PB.set_frozencommission(pDO->FrozenCommission);
	PB.set_currmargin(pDO->CurrMargin);
	PB.set_cashin(pDO->CashIn);
	PB.set_commission(pDO->Commission);
	PB.set_closeprofit(pDO->CloseProfit);
	PB.set_positionprofit(pDO->PositionProfit);
	PB.set_balance(pDO->Balance);
	PB.set_available(pDO->Available);
	PB.set_withdrawquota(pDO->WithdrawQuota);
	PB.set_reserve(pDO->Reserve);
	PB.set_tradingday(pDO->TradingDay);
	PB.set_settlementid(pDO->SettlementID);
	PB.set_credit(pDO->Credit);
	PB.set_mortgage(pDO->Mortgage);
	PB.set_exchangemargin(pDO->ExchangeMargin);
	PB.set_deliverymargin(pDO->DeliveryMargin);
	PB.set_exchangedeliverymargin(pDO->ExchangeDeliveryMargin);
	PB.set_reservebalance(pDO->ReserveBalance);

	int bufSz = PB.ByteSize();
	uint8_t* buff = new uint8_t[bufSz];
	PB.SerializePartialToArray(buff, bufSz);
	return data_buffer(buff, bufSz);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBAccountInfoSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBAccountInfoSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBAccountInfoSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBStringMapSerializer::Instance()->Deserialize(rawdata);
}