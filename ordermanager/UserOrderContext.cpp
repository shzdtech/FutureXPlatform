/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "UserOrderContext.h"

void UserOrderContext::AddOrder(const OrderDO_Ptr & orderDO_Ptr)
{
	auto& usercontract = _userContractOrderMap.getorfill(orderDO_Ptr->UserID());
	std::lock_guard<std::shared_mutex> write_lock(usercontract.mutex());
	_orderIdMap.emplace(orderDO_Ptr->OrderID, orderDO_Ptr);
	usercontract.entry.getorfill(orderDO_Ptr->InstrumentID()).emplace(orderDO_Ptr->OrderID, orderDO_Ptr);
}

void UserOrderContext::AddOrder(const OrderDO& orderDO)
{
	AddOrder(std::make_shared<OrderDO>(orderDO));
}

OrderDO_Ptr UserOrderContext::RemoveOrder(uint64_t orderID)
{
	OrderDO_Ptr ret;
	auto it = _orderIdMap.find(orderID);
	if (it != _orderIdMap.end())
	{
		ret = it->second;
		auto& usercontract = _userContractOrderMap.getorfill(ret->UserID());
		std::lock_guard<std::shared_mutex> write_lock(usercontract.mutex());
		_orderIdMap.erase(it);
		auto& order = *it->second;
		auto& orderMap = GetOrderMapByUserContract(order);
		orderMap.erase(orderID);
	}

	return ret;
}

std::shared_mutex& UserOrderContext::UserMutex(const std::string& userID)
{
	return _userContractOrderMap.getorfill(userID).mutex();
}

std::map<uint64_t, OrderDO_Ptr>& UserOrderContext::GetAllOrder()
{
	return _orderIdMap;
}

std::map<uint64_t, OrderDO_Ptr>& UserOrderContext::GetOrderMapByUserContract(const UserContractKey& userContractID)
{
	return GetOrderMapByUserContract(userContractID.UserID(), userContractID.InstrumentID());
}

std::map<uint64_t, OrderDO_Ptr>& UserOrderContext::GetOrderMapByUserContract(const std::string & userID, const std::string & contractID)
{
	return _userContractOrderMap.getorfill(userID).entry.getorfill(contractID);
}

vector_ptr<OrderDO> UserOrderContext::GetOrdersByUser(const std::string & userID)
{
	auto ret = std::make_shared<std::vector<OrderDO>>();
	auto& usercontract = _userContractOrderMap.getorfill(userID);
	std::shared_lock<std::shared_mutex> read_lock(usercontract.mutex());
	for (auto& uc : usercontract.entry)
	{
		for (auto& c : uc.second)
		{
			ret->push_back(*c.second);
		}
	}

	return ret;
}


OrderDO_Ptr UserOrderContext::FindOrder(uint64_t orderID)
{
	OrderDO_Ptr ret;
	auto it = _orderIdMap.find(orderID);
	if (it != _orderIdMap.end())
		ret = it->second;

	return ret;
}