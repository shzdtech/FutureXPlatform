/***********************************************************************
 * Module:  HedgeOrderManager.h
 * Author:  milk
 * Modified: 2016年1月15日 23:39:13
 * Purpose: Declaration of the class HedgeOrderManager
 ***********************************************************************/

#if !defined(__ordermanager_HedgeOrderManger_h)
#define __ordermanager_HedgeOrderManger_h

#include "AutoOrderManager.h"
#include "MarketPositionContext.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/PortfolioDO.h"
#include <libcuckoo/cuckoohash_map.hh>
#include <atomic>

#include "ordermgr_export.h"

class ORDERMGR_CLASS_EXPORT HedgeOrderManager : public AutoOrderManager
{
public:
	HedgeOrderManager(IOrderAPI* pOrderAPI, 
		const IPricingDataContext_Ptr& pricingCtx,
		const IUserPositionContext_Ptr& exchangePositionCtx);

	void Hedge(const PortfolioKey& portfolioKey);

	OrderDO_Ptr CancelOrder(OrderRequestDO & orderReq);

	int OnMarketOrderUpdated(OrderDO & orderInfo);

	void TradeByStrategy(const StrategyContractDO& strategyDO);

protected:

	cuckoohash_map<PortfolioKey, bool, PortfolioKeyHash> _updatingPortfolioLock;
private:

};

typedef std::shared_ptr<HedgeOrderManager> HedgeOrderManager_Ptr;

#endif