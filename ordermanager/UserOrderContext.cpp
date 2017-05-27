/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "UserOrderContext.h"

UserOrderContext::UserOrderContext()
	: _orderIdMap(2048), _userContractOrderMap(1024), _limitOrderCount(1024)
{
}

void UserOrderContext::UpsertOrder(uint64_t orderID, const OrderDO_Ptr & orderDO_Ptr)
{
	if (_orderIdMap.insert(orderID, orderDO_Ptr))
	{
		if (orderDO_Ptr->TIF != OrderTIFType::IOC)
		{
			_limitOrderCount.upsert(orderDO_Ptr->InstrumentID(), [](int& cnt)
			{
				cnt++;
			}, 1);
		}
	}
	else
	{
		_orderIdMap.update_fn(orderID, [&orderDO_Ptr](OrderDO_Ptr& order_ptr) { *order_ptr = *orderDO_Ptr; });
	}

	if (!_userContractOrderMap.contains(orderDO_Ptr->UserID()))
		_userContractOrderMap.insert(orderDO_Ptr->UserID(), std::move(OrderContractInnerMapType(true)));

	_userContractOrderMap.update_fn(orderDO_Ptr->UserID(), [this, orderID, &orderDO_Ptr](OrderContractInnerMapType& orderMap)
	{
		if (!orderMap.map()->contains(orderDO_Ptr->InstrumentID()))
			orderMap.map()->insert(orderDO_Ptr->InstrumentID(), std::move(OrderIDInnerMapType(true)));

		OrderIDInnerMapType wrapper;
		if (orderMap.map()->find(orderDO_Ptr->InstrumentID(), wrapper))
		{
			wrapper.map()->upsert(orderID,
				[&orderDO_Ptr](OrderDO_Ptr& orderptr)
			{
				*orderptr = *orderDO_Ptr;
			},
				orderDO_Ptr);
		}
	});
}

void UserOrderContext::UpsertOrder(uint64_t orderID, const OrderDO& orderDO)
{
	UpsertOrder(orderID, std::make_shared<OrderDO>(orderDO));
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
		if (_orderIdMap.erase(orderID))
		{
			if (ret->TIF != OrderTIFType::IOC)
			{
				_limitOrderCount.update_fn(ret->InstrumentID(), [](int& cnt)
				{
					cnt--;
				});
			}
		}

		OrderContractInnerMapType orderMap;
		if (_userContractOrderMap.find(ret->UserID(), orderMap))
		{
			OrderIDInnerMapType orders;
			if (orderMap.map()->find(ret->InstrumentID(), orders))
			{
				orders.map()->erase(orderID);
			}
		}
	}

	return ret;
}

OrderIDMapType& UserOrderContext::GetAllOrder()
{
	return _orderIdMap;
}

UserOrderMapType& UserOrderContext::UserOrderMap()
{
	return _userContractOrderMap;
}

vector_ptr<OrderDO_Ptr> UserOrderContext::GetOrdersByUser(const std::string & userID)
{
	auto ret = std::make_shared<std::vector<OrderDO_Ptr>>();
	OrderContractInnerMapType orderMap;
	if (_userContractOrderMap.find(userID, orderMap))
	{
		auto lt = orderMap.map()->lock_table();
		for (auto& uc : lt)
		{
			auto ltlt = uc.second.map()->lock_table();
			for (auto& c : ltlt)
			{
				ret->push_back(c.second);
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

int UserOrderContext::GetLimitOrderCount(const std::string & contractID)
{
	int ret = 0;

	_limitOrderCount.find(contractID, ret);

	return ret;
}
