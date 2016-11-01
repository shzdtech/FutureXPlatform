#include "CTSUtility.h"
#include "CTSConvertor.h"

OrderStatusType CTSUtility::CheckOrderStatus(T4::OrderStatus status, T4::OrderChange lastChange)
{
	OrderStatusType ret = OrderStatusType::UNDEFINED;

	switch (lastChange)
	{
	case T4::OrderChange::SubmissionRiskRejected:
	case T4::OrderChange::SubmissionRejected:
		ret = OrderStatusType::OPEN_REJECTED;
		break;
	case T4::OrderChange::TradeCompleted:
		ret = OrderStatusType::ALL_TRADED;
		break;
	case T4::OrderChange::Trade:
		ret = OrderStatusType::PARTIAL_TRADED;
		break;
	case T4::OrderChange::PullRejected:
		ret = OrderStatusType::CANCEL_REJECTED;
		break;
	case T4::OrderChange::PullSuccess:
		ret = OrderStatusType::CANCELED;
		break;
	default:
		if (status == T4::OrderStatus::Held)
			ret = OrderStatusType::OPENED;
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

	auto orderUUIDArray = Guid::Parse(pOrder->UniqueID).ToByteArray();

	auto ret = std::make_shared<OrderDO>(BitConverter::ToUInt64(orderUUIDArray, 0), exchange, contract, userId);
	ret->OrderSysID = BitConverter::ToUInt64(orderUUIDArray, 8);

	ret->Direction = pOrder->BuySell == T4::BuySell::Buy ? DirectionType::BUY : DirectionType::SELL;
	ret->LimitPrice = market->ConvertTicksToDecimal(pOrder->CurrentLimitTicks);
	ret->Volume = pOrder->CurrentVolume;
	ret->StopPrice = market->ConvertTicksToDecimal(pOrder->CurrentStopTicks);
	ret->Active = pOrder->IsWorking;
	ret->OrderStatus = CTSUtility::CheckOrderStatus(pOrder->Status, pOrder->Change);
	ret->VolumeTraded = pOrder->TotalFillVolume;
	ret->VolumeRemain = pOrder->WorkingVolume;
	CTSConvertor::MarshalString(pOrder->SubmitTime.ToString(), ret->InsertTime);
	CTSConvertor::MarshalString(pOrder->Time.ToString(), ret->UpdateTime);
	CTSConvertor::MarshalString(pOrder->StatusDetail, ret->Message);

	return ret;
}