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
	enum HedgeStatus
	{
		HedgingOff = 0,
		UnderDelta,
		Hedging,
		Waiting,
		Hedgded,
	};

	HedgeOrderManager(const IPricingDataContext_Ptr& pricingCtx,
		const IUserPositionContext_Ptr& exchangePositionCtx);

	HedgeStatus Hedge(const PortfolioKey& portfolioKey, IOrderAPI* orderAPI);

	OrderDO_Ptr CancelOrder(OrderRequestDO & orderReq, IOrderAPI* orderAPI);

	void TradeByStrategy(const StrategyContractDO& strategyDO, IOrderAPI* orderAPI);

	int OnMarketOrderUpdated(OrderDO & orderInfo, IOrderAPI* orderAPI);

protected:

	cuckoohash_map<PortfolioKey, bool, PortfolioKeyHash> _updatingPortfolioLock;
private:

};

typedef std::shared_ptr<HedgeOrderManager> HedgeOrderManager_Ptr;

#endif