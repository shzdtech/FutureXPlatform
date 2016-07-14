/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "UserOrderContext.h"

void UserOrderContext::AddOrder(const OrderDO& orderDO)
{
	auto orderptr = std::make_shared<OrderDO>(orderDO);
	_activeOrders.emplace(orderDO.OrderID, orderptr);
	_orderMap.getorfill(orderDO).entry.emplace(orderDO.OrderID, orderptr);
}

int UserOrderContext::RemoveOrder(uint64_t orderID)
{
	int ret = 0;
	auto it = _activeOrders.find(orderID);
	if (it != _activeOrders.end())
	{
		_activeOrders.erase(it);
		auto& order = *it->second;
		auto& orderMap = GetTradingOrderMap(order);
		orderMap.erase(orderID);
		ret++;
	}

	return ret;
}

std::mutex& UserOrderContext::Mutex(const UserContractKey& userContractID)
{
	return _orderMap.getorfill(userContractID).mutex();
}

std::map<uint64_t, OrderDO_Ptr>& UserOrderContext::GetAllOrder()
{
	return _activeOrders;
}

std::map<uint64_t, OrderDO_Ptr>& UserOrderContext::GetTradingOrderMap(const UserContractKey& userContractID)
{
	return _orderMap.getorfill(userContractID).entry;
}

OrderDO_Ptr UserOrderContext::FindOrder(uint64_t orderID)
{
	OrderDO_Ptr ret;
	auto it = _activeOrders.find(orderID);
	if (it != _activeOrders.end())
		ret = it->second;

	return ret;
}