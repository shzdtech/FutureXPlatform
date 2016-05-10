/***********************************************************************
 * Module:  AutoOrderManager.h
 * Author:  milk
 * Modified: 2015年10月22日 23:49:43
 * Purpose: Declaration of the class AutoOrderManager
 ***********************************************************************/

#if !defined(__ordermanager_AutoOrderManager_h)
#define __ordermanager_AutoOrderManager_h

#include "OrderManager.h"
#include "ordermgr_export.h"
#include "UserOrderContext.h"

class ORDERMGR_CLASS_EXPORT AutoOrderManager : public OrderManager
{
public:
	AutoOrderManager(IOrderAPI* pOrderAPI, PricingContext* pricingCtx);

	int Reset();
	int CreateOrder(OrderDO& orderInfo);
	int CancelOrder(OrderDO& orderInfo);
	int RejectOrder(OrderDO& orderInfo);
	int OnOrderUpdated(OrderDO& orderInfo);
	OrderDOVec_Ptr UpdateOrderByStrategy(
		const StrategyContractDO& strategyDO);

protected:
	std::mutex _mutex;

private:

};

#endif