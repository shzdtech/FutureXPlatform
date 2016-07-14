/***********************************************************************
 * Module:  UserOrderContext.h
 * Author:  milk
 * Modified: 2015年10月28日 11:18:40
 * Purpose: Declaration of the class UserOrderContext
 ***********************************************************************/

#if !defined(__ordermanager_OrderContext_h)
#define __ordermanager_OrderContext_h

#include <atomic>
#include "../dataobject/TypedefDO.h"
#include "../dataobject/OrderDO.h"
#include "../utility/autofillmap.h"
#include "../utility/entrywisemutex.h"

class UserOrderContext
{
public:
	void AddOrder(const OrderDO& orderDO);
	int RemoveOrder(uint64_t orderID);
	std::mutex& Mutex(const UserContractKey& userContractID);
	std::map<uint64_t, OrderDO_Ptr>& GetAllOrder();
	std::map<uint64_t, OrderDO_Ptr>& GetTradingOrderMap(const UserContractKey& userContractID);
	OrderDO_Ptr FindOrder(uint64_t orderID);

private:
	std::map<uint64_t, OrderDO_Ptr> _activeOrders;
	UserContractMap<entrywisemutex<std::map<uint64_t, OrderDO_Ptr>, std::mutex>> _orderMap;
};

#endif