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
	AutoOrderManager(IOrderAPI* pOrderAPI, 
		const IPricingDataContext_Ptr& pricingCtx, 
		const IUserPositionContext_Ptr& exchangePositionCtx);

	int Reset();
	OrderDO_Ptr CreateOrder(OrderRequestDO& orderReq);
	OrderDO_Ptr CancelOrder(OrderRequestDO& orderReq);
	OrderDO_Ptr RejectOrder(OrderRequestDO& orderReq);
	int OnMarketOrderUpdated(OrderDO& orderInfo);
	uint64_t ParseOrderID(uint64_t rawOrderId, uint64_t sessionId);

	void TradeByStrategy(const StrategyContractDO& strategyDO);

protected:
	cuckoohash_map<ContractKey, bool, ContractKeyHash> _updatinglock;

	IUserPositionContext_Ptr _exchangePositionCtx;

private:

};

#endif