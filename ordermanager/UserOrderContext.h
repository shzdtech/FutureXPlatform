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

class ORDERMGR_CLASS_EXPORT UserOrderContext
{
public:
	void AddOrder(const OrderDO_Ptr& orderDO_Ptr);
	void AddOrder(const OrderDO& orderDO);
	void Clear(void);
	OrderDO_Ptr RemoveOrder(uint64_t orderID);
	cuckoohash_map<uint64_t, OrderDO_Ptr>& GetAllOrder();
	cuckoohash_map<std::string, cuckoohashmap_wrapper<std::string, cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>>>& UserOrderMap();
	vector_ptr<OrderDO> GetOrdersByUser(const std::string& userID);
	OrderDO_Ptr FindOrder(uint64_t orderID);


private:
	cuckoohash_map<uint64_t, OrderDO_Ptr> _orderIdMap;
	cuckoohash_map<std::string, cuckoohashmap_wrapper<std::string, cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>>> _userContractOrderMap;
};

#endif