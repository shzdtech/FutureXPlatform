#include "CTSUtility.h"
#include "CTSConvertor.h"

OrderStatus CTSUtility::CheckOrderStatus(T4::OrderStatus status, T4::OrderChange lastChange)
{
	OrderStatus ret = OrderStatus::UNDEFINED;

	switch (lastChange)
	{
	case T4::OrderChange::SubmissionRiskRejected:
	case T4::OrderChange::SubmissionRejected:
		ret = OrderStatus::OPEN_REJECTED;
		break;
	case T4::OrderChange::TradeCompleted:
		ret = OrderStatus::ALL_TRADED;
		break;
	case T4::OrderChange::Trade:
		ret = OrderStatus::PARTIAL_TRADED;
		break;
	case T4::OrderChange::PullRejected:
		ret = OrderStatus::CANCEL_REJECTED;
		break;
	case T4::OrderChange::PullSuccess:
		ret = OrderStatus::CANCELED;
		break;
	default:
		if (status == T4::OrderStatus::Held)
			ret = OrderStatus::OPENNING;
	}

	return ret;
}


OrderDO_Ptr CTSUtility::ParseRawOrder(T4::API::Order^ pOrder)
{
	std::string exchange;
	std::string contract;
	std::string userId;

	auto market = pOrder->Market;
	CTSConvertor::MarshalString(market->ExchangeID, exchange);
	CTSConvertor::MarshalString(market->ContractID, contract);
	CTSConvertor::MarshalString(pOrder->UserID, userId);

	auto ret = std::make_shared<OrderDO>(UInt64::Parse(pOrder->Tag), exchange, contract, userId);

	ret->Direction = pOrder->BuySell == T4::BuySell::Buy ? DirectionType::BUY : DirectionType::SELL;
	ret->LimitPrice = market->ConvertTicksToDecimal(pOrder->CurrentLimitTicks);
	ret->Volume = pOrder->CurrentVolume;
	ret->StopPrice = market->ConvertTicksToDecimal(pOrder->CurrentStopTicks);
	ret->OrderSysID = 0;
	ret->Active = pOrder->IsWorking;
	ret->OrderStatus = CTSUtility::CheckOrderStatus(pOrder->Status, pOrder->Change);
	ret->VolumeTraded = pOrder->TotalFillVolume;
	ret->VolumeRemain = pOrder->WorkingVolume;
	CTSConvertor::MarshalString(pOrder->SubmitTime.ToString(), ret->InsertTime);
	CTSConvertor::MarshalString(pOrder->Time.ToString(), ret->UpdateTime);
	CTSConvertor::MarshalString(pOrder->StatusDetail, ret->Message);

	return ret;
}