/***********************************************************************
 * Module:  ContractOrderContext.h
 * Author:  milk
 * Modified: 2017年01月01日 11:18:40
 * Purpose: Declaration of the class ContractOrderContext
 ***********************************************************************/

#if !defined(__ordermanager_ContractOrderContext_h)
#define __ordermanager_ContractOrderContext_h

#include <atomic>
#include "../dataobject/TypedefDO.h"
#include "../dataobject/OrderDO.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "../utility/cuckoohashmap_wrapper.h"
#include "ordermgr_export.h"

class ORDERMGR_CLASS_EXPORT ContractOrderContext
{
public:
	ContractOrderContext();

	void UpsertOrder(uint64_t orderID, const OrderDO_Ptr& orderDO_Ptr);
	void UpsertOrder(uint64_t orderID, const OrderDO& orderDO);
	void Clear(void);
	OrderDO_Ptr RemoveOrder(uint64_t orderID);
	cuckoohash_map<uint64_t, OrderDO_Ptr>& GetAllOrder();
	cuckoohash_map<std::string, cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>>& ContractOrderMap();
	OrderDO_Ptr FindOrder(uint64_t orderID);


private:
	cuckoohash_map<uint64_t, OrderDO_Ptr> _orderIdMap;
	cuckoohash_map<std::string, cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>> _contractOrderMap;
};

#endif