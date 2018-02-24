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
	CTSConvertor::MarshalString(pOrder->SubmitTime.ToString(), ret->InsertTime);
	CTSConvertor::MarshalString(pOrder->Time.ToString(), ret->UpdateTime);
	CTSConvertor::MarshalString(pOrder->StatusDetail, ret->Message);

	return ret;
}

TradeRecordDO_Ptr CTSUtility::ParseRawTrade(T4::API::Order::Trade^ pTrade)
{
	T4::API::Order^ pOrder;
	auto market = pOrder->Market;
	std::string exchangeID;
	std::string instrumentID;
	std::string userId;
	std::string portfolioId;


	CTSConvertor::MarshalString(pTrade->ExchangeTradeID, exchangeID);
	CTSConvertor::MarshalString(market->ContractID, instrumentID);
	CTSConvertor::MarshalString(pTrade->ContraTrader, userId);

	auto ret = std::make_shared<TradeRecordDO>(exchangeID, instrumentID, userId, portfolioId);

	auto tradeUUIDArray = Guid::Parse(pTrade->TradeID).ToByteArray();
	ret->TradeID = BitConverter::ToUInt64(tradeUUIDArray, 8);
	CTSConvertor::MarshalString(pTrade->ContraBroker, ret->BrokerID);
	CTSConvertor::MarshalString(pTrade->ExchangeTime.ToString(), ret->TradeTime);
	ret->Volume = pTrade->ResidualVolume;
	ret->Price = pTrade->Ticks;
	CTSConvertor::MarshalString(pTrade->TradeDate.ToString(), ret->TradeDate);

	return ret;
}

UserPositionExDO_Ptr CTSUtility::ParseRawPosition(T4::API::Position^ pPosition)
{
	std::string exchangeID;
	std::string instrumentID;
	auto market = pPosition->Market;
	CTSConvertor::MarshalString(market->ExchangeID, exchangeID);
	CTSConvertor::MarshalString(market->ContractID, instrumentID);
	auto ret = std::make_shared<UserPositionExDO>(exchangeID, instrumentID);
	// ret->TdProfit = pPosition->PL;
	ret->UseMargin = pPosition->Margin;
	ret->Commission = pPosition->FeesAndCommissions;
	ret->TdCost = pPosition->Market->ConvertTicksToDecimal (Math::Abs(pPosition->TotalBuyFillTicks - pPosition->TotalSellFillTicks));
	ret->TdPosition = pPosition->Buys - pPosition->Sells;
	ret->YdPosition = pPosition->Net - ret->TdPosition;
	return ret;

}

AccountInfoDO_Ptr CTSUtility::ParseRawAccountInfo(T4::API::Account^ pAccount)
{
	auto ret = std::make_shared<AccountInfoDO>();
	CTSConvertor::MarshalString(pAccount->AccountID, ret->AccountID);
	ret->Commission = pAccount->FeesAndCommissions;
	ret->CloseProfit = pAccount->RPL;
	ret->PositionProfit = pAccount->UPL;
	ret->Available = pAccount->AvailableCash;
	ret->Balance = pAccount->Balance;
	ret->ReserveBalance = pAccount->MinBalance;
	ret->ExchangeDeliveryMargin = pAccount->Margin;

	return ret;

}

