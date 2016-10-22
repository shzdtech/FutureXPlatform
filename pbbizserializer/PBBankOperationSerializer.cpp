#include "PBBankOperationSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/BankOpResultDO.h"
#include "../Protos/businessobj.pb.h"

using namespace Micro::Future::Message::Business;

data_buffer PBBankOperationSerializer::Serialize(const dataobj_ptr & abstractDO)
{
	PBBankOpResult PB;
	auto pDO = (BankOpResultDO*)abstractDO.get();
	FillPBHeader(PB, pDO);

	PB.set_accountid(pDO->AccountID);
	PB.set_bankaccount(pDO->BankAccount);
	PB.set_bankacctype(pDO->BankAccType);
	PB.set_bankbranchid(pDO->BankBranchID);
	PB.set_bankid(pDO->BankID);
	PB.set_bankserial(pDO->BankSerial);
	PB.set_brokerbranchid(pDO->BrokerBranchID);
	PB.set_brokerfee(pDO->BrokerFee);
	PB.set_brokerid(pDO->BrokerID);
	PB.set_currencyid(pDO->CurrencyID);
	PB.set_custfee(pDO->CustFee);
	PB.set_futureacctype(pDO->FutureAccType);
	PB.set_futureserial(pDO->FutureSerial);
	PB.set_serialnum(pDO->SerialNum);
	PB.set_tradeamount(pDO->TradeAmount);
	PB.set_tradecode(pDO->TradeCode);
	PB.set_tradingday(pDO->TradingDay);
	PB.set_tradedate(pDO->TradeDate);
	PB.set_tradetime(pDO->TradeTime);

	if (!pDO->ErrorID)
		PB.set_errorid(pDO->ErrorID);

	if (!pDO->ErrorMsg.empty())
		PB.set_errormsg(pDO->ErrorMsg);

	SerializeWithReturn(PB);
}

dataobj_ptr PBBankOperationSerializer::Deserialize(const data_buffer & rawdata)
{
	PBBankOpRequest PB;
	ParseWithReturn(PB, rawdata);

	auto ret = std::make_shared<BankOpRequestDO>();
	FillDOHeader(ret, PB);

	ret->AccountID = PB.accountid();
	ret->BankAccount = PB.bankaccount();
	ret->BankBranchID = PB.bankbranchid();
	ret->BankID = PB.bankid();
	ret->BankPassword = PB.bankpassword();
	ret->BrokerBranchID = PB.brokerbranchid();
	ret->BrokerID = PB.brokerid();
	ret->CurrencyID = PB.currencyid();
	ret->Password = PB.password();
	ret->TradeAmount = PB.tradeamount();

	return ret;
}
