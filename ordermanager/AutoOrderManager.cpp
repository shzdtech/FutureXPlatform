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

	if (orderInfo.OrderID == 0 || !FindOrder(ParseOrderID(orderInfo.OrderID)))
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
		OrderDOVec_Ptr ret;
		cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr> orders;

		if (!_contractOrderCtx.ContractOrderMap().find(strategyDO.InstrumentID(), orders))
		{
			_contractOrderCtx.ContractOrderMap().insert(strategyDO.InstrumentID(), cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>(true, strategyDO.Depth * 2));
			_contractOrderCtx.ContractOrderMap().find(strategyDO.InstrumentID(), orders);
		}

		IPricingDO_Ptr pricingDO_ptr;
		if (_pricingCtx->GetPricingDataDOMap()->find(strategyDO, pricingDO_ptr))
		{
			ret = std::make_shared<VectorDO<OrderDO>>();

			int depth = strategyDO.Depth;
			bool askEnable = strategyDO.AskEnabled;
			bool bidEnable = strategyDO.BidEnabled;
			double pricingbuyMax = pricingDO_ptr->Bid().Price;
			double pricingbuyMin = pricingbuyMax - (depth - 1)*strategyDO.TickSize;
			double pricingsellMin = pricingDO_ptr->Ask().Price;
			double pricingsellMax = pricingsellMin + (depth - 1)*strategyDO.TickSize;

			std::vector<epsdouble> tlBuyPrices;
			std::vector<epsdouble> tlSellPrices;
			std::vector<uint64_t> orderCancelList;

			auto tradingOrders = orders.map()->lock_table();
			for (auto& pair : tradingOrders)
			{
				auto& order = *pair.second;
				bool canceled = false;
				if (order.Direction == DirectionType::SELL)
				{
					if (askEnable)
					{
						if (order.LimitPrice < pricingsellMin || order.LimitPrice > pricingsellMax)
						{
							canceled = true;
						}
						else
						{
							tlSellPrices.push_back(order.LimitPrice);
						}
					}
					else
					{
						canceled = true;
					}
				}
				else
				{
					if (bidEnable)
					{
						if (order.LimitPrice > pricingbuyMax || order.LimitPrice < pricingbuyMin)
						{
							canceled = true;
						}
						else
						{
							tlBuyPrices.push_back(order.LimitPrice);
						}
					}
					else
					{
						canceled = true;
					}
				}

				if (canceled)
				{
					if (auto order_ptr = _pOrderAPI->CancelOrder(order))
					{
						order.OrderStatus = OrderStatusType::CANCELED;
						orderCancelList.push_back(pair.first);
						ret->push_back(order);
					}
				}
			}
			tradingOrders.release();

			for (auto orderId : orderCancelList)
				_contractOrderCtx.RemoveOrder(orderId);

			// Make new orders
			OrderRequestDO newOrder(strategyDO);

			double sellPrice = pricingsellMin;
			double buyPrice = pricingbuyMax;
			double tickSize = strategyDO.TickSize;

			for (int i = 0; i < strategyDO.Depth; i++)
			{
				if (askEnable && std::find(tlSellPrices.begin(), tlSellPrices.end(), sellPrice) == tlSellPrices.end())
				{
					newOrder.OrderID = 0;
					newOrder.Direction = DirectionType::SELL;
					newOrder.Volume = strategyDO.AskQV;
					newOrder.LimitPrice = sellPrice;
					CreateOrder(newOrder);
					ret->push_back(newOrder);
				}

				if (bidEnable && std::find(tlBuyPrices.begin(), tlBuyPrices.end(), buyPrice) == tlBuyPrices.end())
				{
					newOrder.OrderID = 0;
					newOrder.Direction = DirectionType::BUY;
					newOrder.Volume = strategyDO.BidQV;
					newOrder.LimitPrice = buyPrice;
					CreateOrder(newOrder);
					ret->push_back(newOrder);
				}

				sellPrice += tickSize;
				buyPrice -= tickSize;
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

	auto orderId = ParseOrderID(orderInfo.OrderID);

	if (auto order_ptr = FindOrder(orderId))
	{
		if (order_ptr->OrderSysID == 0 && orderInfo.OrderSysID != 0)
			order_ptr->OrderSysID = orderInfo.OrderSysID;

		bool needUpdaing = false;
		switch (orderInfo.OrderStatus)
		{
		case OrderStatusType::ALL_TRADED:
		case OrderStatusType::CANCELED:
			needUpdaing = true;
			break;
		case OrderStatusType::PARTIAL_TRADING:
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

uint64_t AutoOrderManager::ParseOrderID(uint64_t rawOrderId)
{
	uint64_t sessionId = _pOrderAPI->GetSessionId();
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
			_contractOrderCtx.RemoveOrder(orderInfo.OrderID);
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
				orderCancelList.push_back(pair.first);
			}
			
			for (auto orderId : orderCancelList)
				_contractOrderCtx.RemoveOrder(orderId);
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