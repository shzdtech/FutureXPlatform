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
#include "ordermgr_export.h"

class ORDERMGR_CLASS_EXPORT UserOrderContext
{
public:
	void AddOrder(const OrderDO_Ptr& orderDO_Ptr);
	void AddOrder(const OrderDO& orderDO);
	OrderDO_Ptr RemoveOrder(uint64_t orderID);
	std::shared_mutex& UserMutex(const std::string& userID);
	std::map<uint64_t, OrderDO_Ptr>& GetAllOrder();
	std::map<uint64_t, OrderDO_Ptr>& GetOrderMapByUserContract(const UserContractKey& userContractID);
	std::map<uint64_t, OrderDO_Ptr>& GetOrderMapByUserContract(const std::string& userID, const std::string& contractID);
	vector_ptr<OrderDO> GetOrdersByUser(const std::string& userID);

	OrderDO_Ptr FindOrder(uint64_t orderID);

private:
	std::map<uint64_t, OrderDO_Ptr> _orderIdMap;
	autofillmap<std::string, entrywisemutex<autofillmap<std::string, autofillmap<uint64_t, OrderDO_Ptr>>, std::shared_mutex>> _userContractOrderMap;
};

#endif