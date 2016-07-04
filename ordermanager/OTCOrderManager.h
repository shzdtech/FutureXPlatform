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

class ORDERMGR_CLASS_EXPORT OTCOrderManager : public OrderManager, public IOrderListener
{
public:
	OTCOrderManager(IOrderAPI* pOrderAPI, IPricingDataContext* pricingCtx);

	int Reset();
	int CreateOrder(OrderDO& orderInfo);

	int CancelOrder(OrderDO& orderInfo);
	int RejectOrder(OrderDO& orderInfo);
	int OnOrderUpdated(OrderDO& orderInfo);

	OrderDOVec_Ptr UpdateOrderByStrategy(const StrategyContractDO& strategyDO);

	void Hedge(const PortfolioKey& portfolioKey);

protected:
	HedgeOrderManager_Ptr FindHedgeManager(const PortfolioKey& portfolioKey);

	std::mutex _mutex;
	OTCUserPositionContext _positionCtx;
	PortfolioMap<HedgeOrderManager_Ptr> _hedgeMgr;

private:
	HedgeOrderManager_Ptr initHedgeOrderMgr(const std::string& userID);
};

#endif