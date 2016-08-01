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


AutoOrderManager::AutoOrderManager(IOrderAPI* pOrderAPI, IPricingDataContext* pricingCtx)
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

int AutoOrderManager::CreateOrder(OrderDO& orderInfo)
{
	int ret = 0;

	auto orderId = orderInfo.OrderID;
	if (orderId == 0 || !FindOrder(orderId))
	{
		orderInfo.OrderID = OrderSeqGen::GetNextSeq();
		OrderStatus currStatus;
		auto errCode = _pOrderAPI->CreateOrder(orderInfo, currStatus);
		orderInfo.OrderStatus = currStatus;
		if (errCode == 0)
		{
			_userOrderCtx.AddOrder(orderInfo);
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

OrderDOVec_Ptr AutoOrderManager::UpdateOrderByStrategy(
	const StrategyContractDO& strategyDO)
{
	OrderDOVec_Ptr ret;

	if (strategyDO.Trading)
	{
		std::lock_guard<std::mutex> guard(_userOrderCtx.Mutex(strategyDO));

		if (auto pricingDO_ptr = PricingUtility::Pricing(&strategyDO.Quantity, strategyDO, *_pricingCtx))
		{
			ret = std::make_shared<VectorDO<OrderDO>>();

			int depth = strategyDO.Depth;
			double pricingbuyMax = pricingDO_ptr->BidPrice;
			double pricingbuyMin = pricingbuyMax - (depth - 1)*strategyDO.TickSize;
			double pricingsellMin = pricingDO_ptr->AskPrice;
			double pricingsellMax = pricingsellMin + (depth - 1)*strategyDO.TickSize;

			std::vector<double> tlBuyPrices;
			std::vector<double> tlSellPrices;

			auto& tradingOrders = _userOrderCtx.GetTradingOrderMap(strategyDO);

			for (auto it = tradingOrders.begin(); it != tradingOrders.end();)
			{
				auto& order = *it->second;
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
					OrderStatus currStatus;
					_pOrderAPI->CancelOrder(order, currStatus);
					order.OrderStatus = currStatus;
					it = tradingOrders.erase(it);
					ret->push_back(order);
				}
				else
				{
					it++;
				}
			}

			// Make new orders
			OrderDO newOrder(strategyDO);
			newOrder.Volume = strategyDO.Quantity;
			
			double sellPrice = pricingsellMin;
			double buyPrice = pricingbuyMax;
			double tickSize = strategyDO.TickSize;
			
			for (int i = 0; i < strategyDO.Depth; i++)
			{
				if (std::find(tlSellPrices.begin(), tlSellPrices.end(), sellPrice)
					== tlSellPrices.end())
				{
					newOrder.OrderID = 0;
					newOrder.Direction = DirectionType::SELL;
					newOrder.LimitPrice = sellPrice;
					CreateOrder(newOrder);
					ret->push_back(newOrder);
				}

				if (std::find(tlBuyPrices.begin(), tlBuyPrices.end(), buyPrice)
					== tlBuyPrices.end())
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
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       AutoOrderManager::OnStatusChanged(OrderDO orderInfo)
// Purpose:    Implementation of AutoOrderManager::OnStatusChanged()
// Parameters:
// - orderInfo
// Return:     int
////////////////////////////////////////////////////////////////////////

int AutoOrderManager::OnOrderUpdated(OrderDO& orderInfo)
{
	int ret = -1;

	if (auto order_ptr = FindOrder(orderInfo.OrderID))
	{
		if (order_ptr->OrderSysID == 0 && orderInfo.OrderSysID != 0)
			order_ptr->OrderSysID = orderInfo.OrderSysID;

		bool addnew = false;
		switch (orderInfo.OrderStatus)
		{
		case OrderStatus::ALL_TRADED:
		case OrderStatus::CANCELED:
			addnew = true;
			break;
		case OrderStatus::PARTIAL_TRADING:
			break;
		default:
			break;
		}

		if (!orderInfo.Active)
		{
			std::lock_guard<std::mutex> guard(_userOrderCtx.Mutex(orderInfo));
			_userOrderCtx.RemoveOrder(orderInfo.OrderID);
		}

		if (addnew)
		{
			auto pMap = _pricingCtx->GetStrategyMap();
			auto it = pMap->find(orderInfo);
			if (it != pMap->end())
				UpdateOrderByStrategy(it->second);
		}

		ret = 0;
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::CancelOrder(const OrderDO& orderInfo)
// Purpose:    Implementation of OTCOrderManager::CancelOrder()
// Parameters:
// - orderInfo
// Return:     int
////////////////////////////////////////////////////////////////////////

int AutoOrderManager::CancelOrder(OrderDO& orderInfo)
{
	int ret = -1;
	auto& orderMap = _userOrderCtx.GetTradingOrderMap(orderInfo);
	if (orderInfo.OrderID != 0)
	{
		auto it = orderMap.find(orderInfo.OrderID);
		if (it != orderMap.end())
		{
			it->second->OrderStatus = OrderStatus::CANCELING;
			OrderStatus currStatus;
			ret = _pOrderAPI->CancelOrder(orderInfo, currStatus);
			orderInfo.OrderStatus = currStatus;
			orderMap.erase(it);
		}
	}
	else
	{
		for (auto it = orderMap.begin(); it != orderMap.end();)
		{
			OrderStatus currStatus;
			ret = _pOrderAPI->CancelOrder(*it->second, currStatus);
			it = orderMap.erase(it);
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

int AutoOrderManager::RejectOrder(OrderDO& orderInfo)
{
	return CancelOrder(orderInfo);
}

int AutoOrderManager::Reset()
{
	std::lock_guard<std::mutex> scopelock(_mutex);

	auto& orderMap = _userOrderCtx.GetAllOrder();
	for (auto& it : orderMap)
	{
		RejectOrder(*(it.second));
		_userOrderCtx.RemoveOrder(it.second->OrderID);
	}

	return 0;
}