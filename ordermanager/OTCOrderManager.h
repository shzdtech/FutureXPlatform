/***********************************************************************
 * Module:  OTCOrderManager.h
 * Author:  milk
 * Modified: 2015年10月22日 23:55:11
 * Purpose: Declaration of the class OTCOrderManager
 ***********************************************************************/

#if !defined(__ordermanager_OTCOrderManager_h)
#define __ordermanager_OTCOrderManager_h

#include "OrderManager.h"
#include "HedgeOrderManager.h"
#include "OTCUserPositionContext.h"
#include "ordermgr_export.h"

class ORDERMGR_CLASS_EXPORT OTCOrderManager : public OrderManager
{
public:
	OTCOrderManager(IOrderAPI* pOrderAPI, const IPricingDataContext_Ptr& pricingCtx, IOrderUpdatedEvent* listener);

	int Reset();
	OrderDO_Ptr CreateOrder(OrderRequestDO& orderInfo);
	OrderDO_Ptr CancelOrder(OrderRequestDO& orderInfo);
	OrderDO_Ptr RejectOrder(OrderRequestDO& orderInfo);
	int OnMarketOrderUpdated(OrderDO& orderInfo);

	void TradeByStrategy(const StrategyContractDO& strategyDO);

	OTCUserPositionContext& GetPositionContext();

	//void Hedge(const PortfolioKey& portfolioKey);

protected:
	// HedgeOrderManager_Ptr FindHedgeManager(const PortfolioKey& portfolioKey);

	std::mutex _mutex;
	OTCUserPositionContext _positionCtx;

	cuckoohash_map<ContractKey, bool, ContractKeyHash> _updatinglock;
private:

};

#endif