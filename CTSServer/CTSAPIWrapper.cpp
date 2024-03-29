#include "CTSAPIWrapper.h"
#include "CTSConstant.h"
#include "CTSUtility.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserPositionDO.h"

#include "../ordermanager/OrderSeqGen.h"
#include "../message/BizError.h"
#include "../common/BizErrorIDs.h"

using namespace System;

CTSAPIWrapperImpl::CTSAPIWrapperImpl(IMessageProcessor* pMsgProcessor,
	const std::map<std::string, std::string>& configMap)
{
	_pMsgProcessor = pMsgProcessor;
	_configMap = new std::map<std::string, std::string>(configMap);
	_contractMap = new std::map<ContractKey, gcroot<Market^>>();
	_loginQueue = new std::queue<uint32_t>;
}

CTSAPIWrapperImpl::~CTSAPIWrapperImpl()
{
	delete _configMap;
	delete _contractMap;
	delete _loginQueue;
}

void CTSAPIWrapperImpl::Login(const char* broker, const char* user, const char* passowrd, uint32_t serailId)
{
	_loginQueue->push(serailId);
	auto appName = UUID_MICROFUTURE_CTS;
	auto license = _configMap->at(CTS_LICENSE).data();
	auto servertype = _configMap->at(CTS_SERVER_TYPE);

	int stype = std::stoi(servertype);

	_host = gcnew Host((APIServerType)stype, gcnew String(appName), gcnew String(license),
		gcnew String(broker), gcnew String(user), gcnew String(passowrd));
	_host->LoginSuccess +=
		gcnew Host::LoginSuccessEventHandler(this, &CTSAPIWrapperImpl::OnLoginSuccess);
	_host->LoginFailure +=
		gcnew Host::LoginFailureEventHandler(this, &CTSAPIWrapperImpl::OnLoginFailure);
}

bool CTSAPIWrapperImpl::IsLogged()
{
	return _isLogged;
}

int CTSAPIWrapperImpl::Subscribe(const char* exchangeID, const char* contractID, uint32_t serailId)
{
	return Subscribe(ContractKey(exchangeID, contractID), serailId);
}

int CTSAPIWrapperImpl::Subscribe(const ContractKey& contractKey, uint32_t serailId)
{
	if (!IsSubscribed(contractKey))
	{
		System::String^ exchange = gcnew System::String(contractKey.ExchangeID().data());
		System::String^ contract = gcnew System::String(contractKey.InstrumentID().data());
		auto marketList = _host->MarketData->CreateMarketFilter(exchange, contract);

		while (!marketList->Complete)
		{
			Threading::Thread::Sleep(50);
		}

		if (marketList->Count == 0)
		{
			throw NotFoundException();
		}
		gcroot<Market^> market = marketList[0];
		_contractMap->emplace(contractKey, market);

		market->MarketDepthUpdate +=
			gcnew Market::MarketDepthUpdateEventHandler(this, &CTSAPIWrapperImpl::OnMarketDataUpdated);
		market->MarketHighLow +=
			gcnew Market::MarketHighLowEventHandler(this, &CTSAPIWrapperImpl::OnMarketDataUpdated);
		market->MarketSettlement +=
			gcnew Market::MarketSettlementEventHandler(this, &CTSAPIWrapperImpl::OnMarketDataUpdated);
		//marketList->MarketTradeVolume += 
		//	gcnew MarketList::MarketTradeVolumeEventHandler(_callback, &CTSMDCallback::OnMarketDataUpdated);
	}

	return NO_ERROR_ID;
}

bool CTSAPIWrapperImpl::IsSubscribed(const ContractKey& contractKey)
{
	return _contractMap->find(contractKey) != _contractMap->end();
}

int CTSAPIWrapperImpl::CreateOrder(OrderRequestDO& orderDO)
{
	int ret = Subscribe(orderDO, orderDO.SerialId);

	if (ret <= 0)
	{
		auto it = _contractMap->find(orderDO);

		if (it != _contractMap->end())
		{
			auto account = gcnew OrderList::AccountRouting();
			account->CustomerReference = gcnew String(orderDO.UserID().data());

			auto& market = it->second;
			auto pOrder = _account->SubmitNewOrder
			(
				market,
				orderDO.Direction == DirectionType::SELL ? BuySell::Sell : BuySell::Buy,
				PriceType::Limit,
				orderDO.TIF == OrderTIFType::GFD ? TimeType::GoodTillCancelled : TimeType::ImmediateAndCancel,
				orderDO.Volume,
				market->ConvertDecimalToTicks(orderDO.LimitPrice),
				0,
				OpenClose::Undefined,
				account->CustomerReference,
				0,
				ActivationType::Immediate,
				String::Empty,
				nullptr,
				account
			);

			pOrder->OrderUpdate +=
				gcnew Order::OrderUpdateEventHandler(this, &CTSAPIWrapperImpl::OnOrderUpdated);

			auto orderUUID = Guid::Parse(pOrder->UniqueID);

			_orderMap->Add(orderUUID, pOrder);

			OrderDO_Ptr order_ptr = CTSUtility::ParseRawOrder(pOrder);

			OnResponseProcMacro(_pMsgProcessor, MSG_ID_ORDER_NEW, orderDO.SerialId, &order_ptr);

			ret = NO_ERROR_ID;
		}
	}

	return ret;

}

int CTSAPIWrapperImpl::CancelOrder(OrderRequestDO& orderDO)
{
	int ret = -1;

	Order^ order;

	auto uuidArray = BitConverter::GetBytes(orderDO.OrderID);
	auto uuidArray8 = BitConverter::GetBytes(orderDO.OrderSysID);

	auto orderUUID = Guid(BitConverter::ToInt32(uuidArray, 0), BitConverter::ToInt16(uuidArray, 4), BitConverter::ToInt16(uuidArray, 6),
		uuidArray8);

	if (_orderMap->TryGetValue(orderUUID, order))
	{
		if (order->Pull())
			ret = 0;
		else if (!order->IsWorking)
		{
			throw BizException(OBJECT_IS_CLOSED);
		}
		orderDO = *CTSUtility::ParseRawOrder(order);
	}

	OnResponseProcMacro(_pMsgProcessor, MSG_ID_ORDER_CANCEL, orderDO.SerialId, &orderDO);
	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// CTSAPICallback members
/////////////////////////////////////////////////////////////////////////////////////////////////////


void CTSAPIWrapperImpl::OnLoginSuccess()
{
	_isLogged = true;
	_account = _host->Accounts[0];
	_account->Subscribe();

	int errCode = 0;
	OnResponseProcMacro(_pMsgProcessor, MSG_ID_LOGIN, _loginQueue->front(), &errCode);
	_loginQueue->pop();
}

void CTSAPIWrapperImpl::OnLoginFailure(LoginResult loginResult)
{
	if (loginResult != LoginResult::UnexpectedDisconnect)
	{
		_isLogged = false;
		int errCode = (int)loginResult;
		OnResponseProcMacro(_pMsgProcessor, MSG_ID_LOGIN, _loginQueue->front(), &errCode);
		_loginQueue->pop();
	}
}

void CTSAPIWrapperImpl::OnMarketDataUpdated(Market^ poMarket)
{
	std::string exchange;
	std::string contract;
	CTSConvertor::MarshalString(poMarket->ExchangeID, exchange);
	CTSConvertor::MarshalString(poMarket->ContractID, contract);
	MarketDataDO mdDO(exchange, contract);
	mdDO.Bid().Volume = poMarket->LastDepth->Bids[0]->Volume;
	mdDO.Bid().Price = poMarket->LastDepth->Bids[0]->Ticks;
	mdDO.Ask().Volume = poMarket->LastDepth->Offers[0]->Volume;
	mdDO.Ask().Price = poMarket->LastDepth->Offers[0]->Ticks;
	mdDO.LastPrice = poMarket->StrikePrice.Value;

	mdDO.HighestPrice = poMarket->ConvertTicksToDecimal(poMarket->LastHighLow->HighTicks);
	mdDO.LowestPrice = poMarket->ConvertTicksToDecimal(poMarket->LastHighLow->LowTicks);
	mdDO.SettlementPrice = poMarket->ConvertTicksToDecimal(poMarket->LastSettlement->Ticks);
	mdDO.Volume = poMarket->LastTradeVolume->Count;

	OnResponseProcMacro(_pMsgProcessor, MSG_ID_RET_MARKETDATA, 0, &mdDO);
}


void CTSAPIWrapperImpl::OnOrderUpdated(Order^ pOrder)
{
	OrderDO_Ptr order_ptr = CTSUtility::ParseRawOrder(pOrder);

	if (order_ptr)
	{
		int msgId;

		switch (order_ptr->OrderStatus)
		{
		case ::OrderStatusType::CANCELED:
		case ::OrderStatusType::CANCEL_REJECTED:
			msgId = MSG_ID_ORDER_CANCEL;
			break;
		case ::OrderStatusType::OPENED:
		case ::OrderStatusType::OPEN_REJECTED:
			msgId = MSG_ID_ORDER_NEW;
			break;
		case ::OrderStatusType::PARTIAL_TRADING:
		case ::OrderStatusType::ALL_TRADED:
			msgId = MSG_ID_ORDER_UPDATE;
			break;
		default:
			return;
		}

		OnResponseProcMacro(_pMsgProcessor, msgId, 0, order_ptr.get());
	}
}

void CTSAPIWrapperImpl::OnPositionUpdated(Position^ poPosition)
{
	UserPositionExDO_Ptr position_ptr = CTSUtility::ParseRawPosition(poPosition);
	OnResponseProcMacro(_pMsgProcessor, MSG_ID_POSITION_UPDATED, 0, position_ptr.get());

}

void CTSAPIWrapperImpl::OnTradeUpdated(T4::API::Order::Trade^ poTrade)
{
	TradeRecordDO_Ptr trade_ptr = CTSUtility::ParseRawTrade(poTrade);
	OnResponseProcMacro(_pMsgProcessor, MSG_ID_TRADE_RTN, 0, trade_ptr.get());

}

void CTSAPIWrapperImpl::OnAccountInfoUpdated(Account^ poAccount)
{
	AccountInfoDO_Ptr account_ptr = CTSUtility::ParseRawAccountInfo(poAccount);
	// OnResponseProcMacro(_pMsgProcessor, MSG_ID_TRADE_RTN, 0, &(*account_ptr));

}


VectorDO_Ptr<OrderDO> CTSAPIWrapperImpl::QueryOrder()
{

	auto vectorPtr = std::make_shared<VectorDO<OrderDO>>();

	for each (T4::API::Position ^position in _account->Positions)
	{
		for each (T4::API::Order ^order in position->Orders)
		{
			OrderDO_Ptr order_ptr = CTSUtility::ParseRawOrder(order);
			vectorPtr->push_back(*order_ptr);
		}
	}
	return vectorPtr;
}

VectorDO_Ptr<TradeRecordDO> CTSAPIWrapperImpl::QueryOTCUserTrades()
{

	auto vectorPtr = std::make_shared<VectorDO<TradeRecordDO>>();

	for each (T4::API::Position ^position in _account->Positions)
	{
		for each (T4::API::Order ^order in position->Orders)
		{
			for each (T4::API::Order::Trade ^trade in order->Trades->Trades)
			{
				TradeRecordDO_Ptr trade_ptr = CTSUtility::ParseRawTrade(trade);
				vectorPtr->push_back(*trade_ptr);
			}
		}
	}
	return vectorPtr;
}

VectorDO_Ptr<UserPositionExDO> CTSAPIWrapperImpl::QueryPosition()
{

	auto vectorPtr = std::make_shared<VectorDO<UserPositionExDO>>();
	for each (T4::API::Position ^position in _account->Positions)
	{
		UserPositionExDO_Ptr position_ptr = CTSUtility::ParseRawPosition(position);
		vectorPtr->push_back(*position_ptr);
	}

	return vectorPtr;
}

VectorDO_Ptr<AccountInfoDO> CTSAPIWrapperImpl::QueryAccount()
{

	auto vectorPtr = std::make_shared<VectorDO<AccountInfoDO>>();

	{
		AccountInfoDO_Ptr account_ptr = CTSUtility::ParseRawAccountInfo(_account);
		vectorPtr->push_back(*account_ptr);
	}

	return vectorPtr;
}

