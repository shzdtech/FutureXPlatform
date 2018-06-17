#if !defined(__ordermanager_OrderReqCache_h)
#define __ordermanager_OrderReqCache_h
#include "ordermgr_export.h"
#include "../dataobject/OrderDO.h"

class ORDERMGR_CLASS_EXPORT OrderReqCache
{
public:
	static bool Insert(uint64_t orderId, const OrderRequestDO& portfolio);
	static bool Find(uint64_t orderId, OrderRequestDO& portfolio);
	static bool Remove(uint64_t orderId);
	static void Clear(void);
};

#endif