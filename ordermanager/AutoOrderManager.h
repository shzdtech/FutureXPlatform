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
#include "IUserPositionContext.h"

class ORDERMGR_CLASS_EXPORT AutoOrderManager : public OrderManager
{
public:
	AutoOrderManager(
		const IPricingDataContext_Ptr& pricingCtx, 
		const IUserPositionContext_Ptr& exchangePositionCtx);

	int CancelUserOrders(const std::string& userId, IOrderAPI* orderAPI);
	OrderDO_Ptr CreateOrder(OrderRequestDO& orderReq, IOrderAPI* orderAPI);
	OrderDO_Ptr CancelOrder(OrderRequestDO& orderReq, IOrderAPI* orderAPI);
	OrderDO_Ptr RejectOrder(OrderRequestDO& orderReq, IOrderAPI* orderAPI);
	void TradeByStrategy(const StrategyContractDO& strategyDO, IOrderAPI* orderAPI);
	int OnMarketOrderUpdated(OrderDO& orderInfo, IOrderAPI* orderAPI);
	uint64_t ParseOrderID(uint64_t rawOrderId, uint64_t sessionId, IOrderAPI* orderAPI);

protected:
	cuckoohash_map<UserContractKey, bool, UserContractKeyHash> _updatinglock;

	IUserPositionContext_Ptr _exchangePositionCtx;

private:

};

#endif