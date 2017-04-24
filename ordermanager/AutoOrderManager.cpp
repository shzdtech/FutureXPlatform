/***********************************************************************
 * Module:  AutoOrderManager.cpp
 * Author:  milk
 * Modified: 2015年10月22日 23:49:43
 * Purpose: Implementation of the class AutoOrderManager
 ***********************************************************************/

#include <thread>
#include <algorithm>
#include "../utility/epsdouble.h"
#include "AutoOrderManager.h"
#include "OrderSeqGen.h"
#include "../databaseop/OrderDAO.h"
#include "../pricingengine/PricingUtility.h"


AutoOrderManager::AutoOrderManager(IOrderAPI* pOrderAPI, const IPricingDataContext_Ptr& pricingCtx)
	: OrderManager(pOrderAPI, pricingCtx), _updatinglock(2048)
{
}

////////////////////////////////////////////////////////////////////////
// Name:       AutoOrderManager::CreateOrder(const OrderDO& orderInfo)
// Purpose:    Implementation of AutoOrderManager::CreateOrder()
// Parameters:
// - orderInfo
// Return:     OrderDO_Ptr
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr AutoOrderManager::CreateOrder(OrderRequestDO& orderInfo)
{
	OrderDO_Ptr ret;

	if (orderInfo.OrderID == 0 || !FindOrder(ParseOrderID(orderInfo.OrderID, 0)))
	{
		orderInfo.OrderID = OrderSeqGen::GenOrderID(_pOrderAPI->GetSessionId());
		_contractOrderCtx.UpsertOrder(orderInfo.OrderID, orderInfo);
		ret = _pOrderAPI->CreateOrder(orderInfo);
		if (!ret)
		{
			_contractOrderCtx.RemoveOrder(orderInfo.OrderID);
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       AutoOrderManager::UpdateByStrategy(const StrategyContractDO& strategyDO)
// Purpose:    Implementation of AutoOrderManager::UpdateStatus()
// Parameters:
// - strategyDO
// Return:     int
////////////////////////////////////////////////////////////////////////

void AutoOrderManager::TradeByStrategy(const StrategyContractDO& strategyDO)
{
	if (!strategyDO.Hedging || strategyDO.IsOTC())
		return;

	_updatinglock.upsert(strategyDO.InstrumentID(), [](bool& lock) { lock = true; }, true);

	_updatinglock.update_fn(strategyDO.InstrumentID(), [this, &strategyDO](bool& lock)
	{
		cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr> orders;

		if (!_contractOrderCtx.ContractOrderMap().find(strategyDO.InstrumentID(), orders))
		{
			_contractOrderCtx.ContractOrderMap().insert(strategyDO.InstrumentID(), cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>(true, strategyDO.Depth * 2));
			_contractOrderCtx.ContractOrderMap().find(strategyDO.InstrumentID(), orders);
		}

		IPricingDO_Ptr pricingDO_ptr;
		if (_pricingCtx->GetPricingDataDOMap()->find(strategyDO, pricingDO_ptr))
		{
			int depth = strategyDO.Depth;
			double tickSize = strategyDO.TickSize;
			bool askEnable = strategyDO.AskEnabled;
			bool bidEnable = strategyDO.BidEnabled;
			double pricingbuyMax = pricingDO_ptr->Bid().Price;
			double pricingbuyMin = pricingbuyMax - (depth - 1)*tickSize;
			double pricingsellMin = pricingDO_ptr->Ask().Price;
			double pricingsellMax = pricingsellMin + (depth - 1)*tickSize;

			autofillmap<epsdouble, int> bidPriceOrders;
			autofillmap<epsdouble, int> askPriceOrders;

			std::vector<OrderDO_Ptr> orderCancelList;

			for (auto& pair : orders.map()->lock_table())
			{
				auto& order = pair.second;
				if (order->Direction == DirectionType::SELL)
				{
					if (askEnable)
					{
						if (order->LimitPrice < pricingsellMin || order->LimitPrice > pricingsellMax)
						{
							_pOrderAPI->CancelOrder(*order);
						}
						else
						{
							int& totalVol = askPriceOrders.getorfill(order->LimitPrice);
							int remain = order->VolumeRemain();
							if ((totalVol + remain) > strategyDO.AskQV)
								orderCancelList.push_back(order);
							else
								totalVol += remain;
						}
					}
					else
					{
						_pOrderAPI->CancelOrder(*order);
					}
				}
				else
				{
					if (bidEnable)
					{
						if (order->LimitPrice > pricingbuyMax || order->LimitPrice < pricingbuyMin)
						{
							_pOrderAPI->CancelOrder(*order);
						}
						else
						{
							int& totalVol = bidPriceOrders.getorfill(order->LimitPrice);
							int remain = order->VolumeRemain();
							if ((totalVol + remain) > strategyDO.BidQV)
								orderCancelList.push_back(order);
							else
								totalVol += remain;
						}
					}
					else
					{
						_pOrderAPI->CancelOrder(*order);
					}
				}
			}

			for (auto& order : orderCancelList)
			{
				_pOrderAPI->CancelOrder(*order);
			}


			//for (auto orderId : orderCancelList)
			//	_contractOrderCtx.RemoveOrder(orderId);

			// Make new orders
			MarketDataDO mdo;
			if (!_pricingCtx->GetMarketDataMap()->find(strategyDO.InstrumentID(), mdo))
			{
				mdo.LowerLimitPrice = 0;
				mdo.HighestPrice = 1e32;
			}

			OrderRequestDO newOrder(strategyDO);

			epsdouble askPrice(pricingsellMin);
			epsdouble bidPrice(pricingbuyMax);

			for (int i = 0; i < strategyDO.Depth; i++)
			{
				if (askEnable)
				{
					auto pQV = askPriceOrders.tryfind(askPrice);
					newOrder.Volume = pQV ? strategyDO.AskQV - *pQV : strategyDO.AskQV;

					if (askPrice <= mdo.UpperLimitPrice && newOrder.Volume > 0)
					{
						newOrder.OrderID = 0;
						newOrder.Direction = DirectionType::SELL;
						newOrder.LimitPrice = askPrice.value();
						CreateOrder(newOrder);
					}
				}

				if (bidEnable)
				{
					auto pQV = bidPriceOrders.tryfind(bidPrice);
					newOrder.Volume = pQV ? strategyDO.BidQV - *pQV : strategyDO.BidQV;

					if (bidPrice >= mdo.LowerLimitPrice && newOrder.Volume > 0)
					{
						newOrder.OrderID = 0;
						newOrder.Direction = DirectionType::BUY;
						newOrder.LimitPrice = bidPrice.value();
						CreateOrder(newOrder);
					}
				}

				askPrice += tickSize;
				bidPrice -= tickSize;
			}
		}

		lock = true;
	});
}

////////////////////////////////////////////////////////////////////////
// Name:       AutoOrderManager::OnStatusChanged(OrderDO orderInfo)
// Purpose:    Implementation of AutoOrderManager::OnStatusChanged()
// Parameters:
// - orderInfo
// Return:     int
////////////////////////////////////////////////////////////////////////

int AutoOrderManager::OnMarketOrderUpdated(OrderDO& orderInfo)
{
	int ret = -1;

	auto orderId = ParseOrderID(orderInfo.OrderID, orderInfo.SessionID);

	if (auto order_ptr = FindOrder(orderId))
	{
		*order_ptr = orderInfo;
		order_ptr->OrderID = orderId;

		bool needUpdaing = false;
		switch (orderInfo.OrderStatus)
		{
		case OrderStatusType::ALL_TRADED:
		case OrderStatusType::CANCELED:
		case OrderStatusType::PARTIAL_TRADING:
			needUpdaing = true;
			break;
		default:
			break;
		}

		if (!orderInfo.Active)
		{
			_contractOrderCtx.RemoveOrder(orderId);
		}

		if (needUpdaing)
		{
			StrategyContractDO_Ptr strategy_ptr;
			if (_pricingCtx->GetStrategyMap()->find(orderInfo, strategy_ptr))
				TradeByStrategy(*strategy_ptr);
		}

		ret = 0;
	}

	return ret;
}

uint64_t AutoOrderManager::ParseOrderID(uint64_t rawOrderId, uint64_t sessionId)
{
	if (!sessionId)
		sessionId = _pOrderAPI->GetSessionId();

	return sessionId << 32 | rawOrderId;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::CancelOrder(const OrderDO& orderInfo)
// Purpose:    Implementation of OTCOrderManager::CancelOrder()
// Parameters:
// - orderInfo
// Return:     int
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr AutoOrderManager::CancelOrder(OrderRequestDO& orderInfo)
{
	OrderDO_Ptr ret;
	if (orderInfo.OrderID != 0)
	{
		if (ret = FindOrder(orderInfo.OrderID))
		{
			ret = _pOrderAPI->CancelOrder(orderInfo);
			// _contractOrderCtx.RemoveOrder(orderInfo.OrderID);
		}
	}
	else
	{
		cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr> orders;
		if (_contractOrderCtx.ContractOrderMap().find(orderInfo.InstrumentID(), orders))
		{
			std::vector<uint64_t> orderCancelList;
			for (auto& pair : orders.map()->lock_table())
			{
				_pOrderAPI->CancelOrder(*pair.second);
				// orderCancelList.push_back(pair.first);
			}

			/*for (auto orderId : orderCancelList)
				_contractOrderCtx.RemoveOrder(orderId);*/
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::RejectOrder(const OrderDO& orderInfo)
// Purpose:    Implementation of OTCOrderManager::RejectOrder()
// Parameters:
// - orderInfo
// Return:     int
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr AutoOrderManager::RejectOrder(OrderRequestDO& orderInfo)
{
	return CancelOrder(orderInfo);
}

int AutoOrderManager::Reset()
{
	auto& orderMap = _contractOrderCtx.GetAllOrder();
	{
		auto lt = orderMap.lock_table();
		for (auto& it : lt)
		{
			CancelOrder(*(it.second));
		}
	}
	_contractOrderCtx.Clear();
	return 0;
}