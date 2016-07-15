#include "CTSAPIWrapper.h"
#include "CTSConstant.h"
#include "CTSUtility.h"
#include "../dataobject/MarketDataDO.h"
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

	return NO_ERROR;
}

bool CTSAPIWrapperImpl::IsSubscribed(const ContractKey& contractKey)
{
	return _contractMap->find(contractKey) != _contractMap->end();
}

int CTSAPIWrapperImpl::CreateOrder(OrderDO& orderDO)
{
	int ret = Subscribe(orderDO, orderDO.SerialId);

	if (ret <= 0)
	{
		auto it = _contractMap->find(orderDO);

		if (it != _contractMap->end())
		{
			UInt64 orderId = OrderSeqGen::GetNextSeq();
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
				orderId.ToString(),
				0,
				ActivationType::Immediate,
				String::Empty,
				nullptr,
				account
			);

			pOrder->OrderUpdate +=
				gcnew Order::OrderUpdateEventHandler(this, &CTSAPIWrapperImpl::OnOrderUpdated);

			_orderMap->Add(orderId, pOrder);

			OrderDO_Ptr order_ptr = CTSUtility::ParseRawOrder(pOrder);

			OnResponseProcMacro(_pMsgProcessor, MSG_ID_ORDER_NEW, orderDO.SerialId, &order_ptr);

			ret = NO_ERROR;
		}
	}

	return ret;

}

int CTSAPIWrapperImpl::CancelOrder(OrderDO& orderDO)
{
	int ret = -1;

	Order^ order;
	if (_orderMap->TryGetValue(orderDO.OrderID, order))
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
	mdDO.BidVolume = poMarket->LastDepth->Bids[0]->Volume;
	mdDO.BidPrice = poMarket->LastDepth->Bids[0]->Ticks;
	mdDO.AskVolume = poMarket->LastDepth->Offers[0]->Volume;
	mdDO.AskPrice = poMarket->LastDepth->Offers[0]->Ticks;
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
		case ::OrderStatus::CANCELED:
		case ::OrderStatus::CANCEL_REJECTED:
			msgId = MSG_ID_ORDER_CANCEL;
			break;
		case ::OrderStatus::OPENNING:
		case ::OrderStatus::OPEN_REJECTED:
			msgId = MSG_ID_ORDER_NEW;
			break;
		case ::OrderStatus::PARTIAL_TRADING:
		case ::OrderStatus::ALL_TRADED:
			msgId = MSG_ID_ORDER_UPDATE;
			break;
		default:
			return;
		}

		OnResponseProcMacro(_pMsgProcessor, msgId, 0, &(*order_ptr));
	}
}
