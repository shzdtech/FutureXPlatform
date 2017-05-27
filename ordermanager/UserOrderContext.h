/***********************************************************************
 * Module:  UserOrderContext.h
 * Author:  milk
 * Modified: 2015年10月28日 11:18:40
 * Purpose: Declaration of the class UserOrderContext
 ***********************************************************************/

#if !defined(__ordermanager_UserOrderContext_h)
#define __ordermanager_UserOrderContext_h

#include <atomic>
#include "../dataobject/TypedefDO.h"
#include "../dataobject/OrderDO.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "../utility/cuckoohashmap_wrapper.h"
#include "ordermgr_export.h"

typedef cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr> OrderIDInnerMapType;
typedef cuckoohashmap_wrapper<std::string, OrderIDInnerMapType> OrderContractInnerMapType;
typedef cuckoohash_map<std::string, OrderContractInnerMapType> UserOrderMapType;

typedef cuckoohash_map<uint64_t, OrderDO_Ptr> OrderIDMapType;

typedef cuckoohash_map<std::string, int> LimitOrderCount;

class ORDERMGR_CLASS_EXPORT UserOrderContext
{
public:
	UserOrderContext();

	void UpsertOrder(uint64_t orderID, const OrderDO_Ptr& orderDO_Ptr);
	void UpsertOrder(uint64_t orderID, const OrderDO& orderDO);
	void Clear(void);
	OrderDO_Ptr RemoveOrder(uint64_t orderID);
	OrderIDMapType& GetAllOrder();
	UserOrderMapType& UserOrderMap();
	vector_ptr<OrderDO_Ptr> GetOrdersByUser(const std::string& userID);
	OrderDO_Ptr FindOrder(uint64_t orderID);
	int GetLimitOrderCount(const std::string& contractID);


private:
	OrderIDMapType _orderIdMap;
	UserOrderMapType _userContractOrderMap;
	LimitOrderCount _limitOrderCount;
};

#endif