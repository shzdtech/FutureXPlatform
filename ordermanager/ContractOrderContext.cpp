/***********************************************************************
 * Module:  ContractOrderContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class ContractOrderContext
 ***********************************************************************/

#include "ContractOrderContext.h"

ContractOrderContext::ContractOrderContext()
	: _orderIdMap(2048), _contractOrderMap(1024)
{
}

void ContractOrderContext::UpsertOrder(uint64_t orderID, const OrderDO_Ptr & orderDO_Ptr)
{
	_orderIdMap.upsert(orderID,
		[&orderDO_Ptr](OrderDO_Ptr& orderptr)
	{
		*orderptr = *orderDO_Ptr;
	}, orderDO_Ptr);

	if (!_contractOrderMap.contains(orderDO_Ptr->InstrumentID()))
		_contractOrderMap.insert(orderDO_Ptr->InstrumentID(), std::move(cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>(true)));

	_contractOrderMap.update_fn(orderDO_Ptr->InstrumentID(), [orderID, &orderDO_Ptr](cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>& orderMap)
	{
		orderMap.map()->upsert(orderID,
			[&orderDO_Ptr](OrderDO_Ptr& orderptr)
		{
			*orderptr = *orderDO_Ptr;
		},
		orderDO_Ptr);
	});
}

void ContractOrderContext::UpsertOrder(uint64_t orderID, const OrderDO& orderDO)
{
	UpsertOrder(orderID, std::make_shared<OrderDO>(orderDO));
}

void ContractOrderContext::Clear(void)
{
	_orderIdMap.clear();
	_contractOrderMap.clear();
}

OrderDO_Ptr ContractOrderContext::RemoveOrder(uint64_t orderID)
{
	OrderDO_Ptr ret;
	if (_orderIdMap.find(orderID, ret))
	{
		_orderIdMap.erase(orderID);

		cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr> orderMap;
		if(_contractOrderMap.find(ret->InstrumentID(), orderMap))
		{
			orderMap.map()->erase(orderID);
		}
	}

	return ret;
}

cuckoohash_map<uint64_t, OrderDO_Ptr>& ContractOrderContext::GetAllOrder()
{
	return _orderIdMap;
}

cuckoohash_map<std::string, cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>>& ContractOrderContext::ContractOrderMap()
{
	return _contractOrderMap;
}


OrderDO_Ptr ContractOrderContext::FindOrder(uint64_t orderID)
{
	OrderDO_Ptr ret;
	_orderIdMap.find(orderID, ret);

	return ret;
}