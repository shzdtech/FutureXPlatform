/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "UserOrderContext.h"

void UserOrderContext::AddOrder(const OrderDO_Ptr & orderDO_Ptr)
{
	if (!_userContractOrderMap.contains(orderDO_Ptr->UserID()))
		_userContractOrderMap.insert(orderDO_Ptr->UserID(), std::move(cuckoohashmap_wrapper<std::string, cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>>{}));

	_userContractOrderMap.update_fn(orderDO_Ptr->UserID(), [&orderDO_Ptr](cuckoohashmap_wrapper<std::string, cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>>& orderMap)
	{
		if (!orderMap.map().contains(orderDO_Ptr->InstrumentID()))
			orderMap.map().insert(orderDO_Ptr->InstrumentID(), std::move(cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>{}));

		orderMap.map().update_fn(orderDO_Ptr->InstrumentID(), [&orderDO_Ptr](cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>& orders)
		{
			orders.map().insert(orderDO_Ptr->OrderID, orderDO_Ptr);
		});
	});

	_orderIdMap.insert(orderDO_Ptr->OrderID, orderDO_Ptr);
}

void UserOrderContext::AddOrder(const OrderDO& orderDO)
{
	AddOrder(std::make_shared<OrderDO>(orderDO));
}

void UserOrderContext::Clear(void)
{
	_orderIdMap.clear();
	_userContractOrderMap.clear();
}

OrderDO_Ptr UserOrderContext::RemoveOrder(uint64_t orderID)
{
	OrderDO_Ptr ret;
	if (_orderIdMap.find(orderID, ret))
	{
		_orderIdMap.erase(orderID);

		cuckoohashmap_wrapper<std::string, cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>> orderMap;
		if(_userContractOrderMap.find(ret->UserID(), orderMap))
		{
			cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr> orders;
			if(orderMap.map().find(ret->InstrumentID(), orders))
			{
				orders.map().erase(ret->OrderID);
			}
		}
	}

	return ret;
}

cuckoohash_map<uint64_t, OrderDO_Ptr>& UserOrderContext::GetAllOrder()
{
	return _orderIdMap;
}

cuckoohash_map<std::string, cuckoohashmap_wrapper<std::string, cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>>>& UserOrderContext::UserOrderMap()
{
	return _userContractOrderMap;
}

vector_ptr<OrderDO> UserOrderContext::GetOrdersByUser(const std::string & userID)
{
	auto ret = std::make_shared<std::vector<OrderDO>>();
	cuckoohashmap_wrapper<std::string, cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>> orderMap;
	if (_userContractOrderMap.find(userID, orderMap))
	{
		auto lt = orderMap.map().lock_table();
		for (auto& uc : lt)
		{
			auto ltlt = uc.second.map().lock_table();
			for (auto& c : ltlt)
			{
				ret->push_back(*c.second);
			}
		}
	}

	return ret;
}


OrderDO_Ptr UserOrderContext::FindOrder(uint64_t orderID)
{
	OrderDO_Ptr ret;
	_orderIdMap.find(orderID, ret);

	return ret;
}