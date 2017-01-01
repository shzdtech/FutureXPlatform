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
	AutoOrderManager(IOrderAPI* pOrderAPI, const IPricingDataContext_Ptr& pricingCtx);

	int Reset();
	OrderDO_Ptr CreateOrder(OrderRequestDO& orderInfo);
	OrderDO_Ptr CancelOrder(OrderRequestDO& orderInfo);
	OrderDO_Ptr RejectOrder(OrderRequestDO& orderInfo);
	int OnMarketOrderUpdated(OrderDO& orderInfo);
	uint64_t ParseOrderID(uint64_t rawOrderId);

	void TradeByStrategy(const StrategyContractDO& strategyDO);

protected:

private:

};

#endif