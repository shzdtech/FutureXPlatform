/***********************************************************************
 * Module:  AutoOrderManager.cpp
 * Author:  milk
 * Modified: 2015年10月22日 23:49:43
 * Purpose: Implementation of the class AutoOrderManager
 ***********************************************************************/

#include <thread>
#include <algorithm>
#include "AutoOrderManager.h"
#include "OrderSeqGen.h"
#include "../databaseop/OrderDAO.h"
#include "../pricingengine/PricingUtility.h"


AutoOrderManager::AutoOrderManager(IOrderAPI* pOrderAPI, const IPricingDataContext_Ptr& pricingCtx)
	: OrderManager(pOrderAPI, pricingCtx)
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

	auto orderId = orderInfo.OrderID;
	if (orderId == 0 || !FindOrder(ParseOrderID(orderId)))
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
	OrderDOVec_Ptr ret;
	std::vector<uint64_t> orderCancelList;

	_contractOrderCtx.ContractOrderMap().update_fn(strategyDO.InstrumentID(), [&](cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>& orders)
	{
		if (auto pricingDO_ptr = PricingUtility::Pricing(nullptr, strategyDO, *_pricingCtx))
		{
			ret = std::make_shared<VectorDO<OrderDO>>();

			int depth = strategyDO.Depth;
			double pricingbuyMax = pricingDO_ptr->Bid().Price;
			double pricingbuyMin = pricingbuyMax - (depth - 1)*strategyDO.TickSize;
			double pricingsellMin = pricingDO_ptr->Ask().Price;
			double pricingsellMax = pricingsellMin + (depth - 1)*strategyDO.TickSize;

			std::vector<double> tlBuyPrices;
			std::vector<double> tlSellPrices;

			auto tradingOrders = orders.map()->lock_table();

			for (auto& pair : tradingOrders)
			{
				auto& order = *pair.second;
				bool canceled = false;
				if (order.Direction == DirectionType::SELL)
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
					if (order.LimitPrice > pricingbuyMax || order.LimitPrice < pricingbuyMin)
					{
						canceled = true;
					}
					else
					{
						tlBuyPrices.push_back(order.LimitPrice);
					}
				}

				if (canceled)
				{
					if (auto order_ptr = _pOrderAPI->CancelOrder(order))
					{
						order.OrderStatus = OrderStatusType::CANCELED;
						orderCancelList.push_back(order.OrderID);
						ret->push_back(order);
					}
				}
			}

			// Make new orders
			OrderRequestDO newOrder(strategyDO);
			newOrder.Volume = strategyDO.BidQT;

			double sellPrice = pricingsellMin;
			double buyPrice = pricingbuyMax;
			double tickSize = strategyDO.TickSize;

			for (int i = 0; i < strategyDO.Depth; i++)
			{
				if (std::find(tlSellPrices.begin(), tlSellPrices.end(), sellPrice) == tlSellPrices.end())
				{
					newOrder.OrderID = 0;
					newOrder.Direction = DirectionType::SELL;
					newOrder.LimitPrice = sellPrice;
					CreateOrder(newOrder);
					ret->push_back(newOrder);
				}

				if (std::find(tlBuyPrices.begin(), tlBuyPrices.end(), buyPrice) == tlBuyPrices.end())
				{
					newOrder.OrderID = 0;
					newOrder.Direction = DirectionType::BUY;
					newOrder.LimitPrice = buyPrice;
					CreateOrder(newOrder);
					ret->push_back(newOrder);
				}

				sellPrice += tickSize;
				buyPrice -= tickSize;
			}
		}
	});

	for (auto orderId : orderCancelList)
	{
		_contractOrderCtx.RemoveOrder(ParseOrderID(orderId));
	}
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

	auto orderId = (uint64_t)orderInfo.SessionID << 32 | orderInfo.OrderID;

	if (auto order_ptr = FindOrder(orderId))
	{
		if (order_ptr->OrderSysID == 0 && orderInfo.OrderSysID != 0)
			order_ptr->OrderSysID = orderInfo.OrderSysID;

		bool addnew = false;
		switch (orderInfo.OrderStatus)
		{
		case OrderStatusType::ALL_TRADED:
		case OrderStatusType::CANCELED:
			addnew = true;
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

		if (addnew)
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
	return ((uint64_t)_pOrderAPI->GetSessionId()) << 32 | rawOrderId;
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
		if (ret = FindOrder(ParseOrderID(orderInfo.OrderID)))
		{
			ret = _pOrderAPI->CancelOrder(orderInfo);
			_contractOrderCtx.RemoveOrder(orderInfo.OrderID);
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
			RejectOrder(*(it.second));
		}
	}
	_contractOrderCtx.Clear();
	return 0;
}