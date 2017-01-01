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
	HedgeOrderManager(const PortfolioKey& portfolio, IOrderAPI* pOrderAPI, const IPricingDataContext_Ptr& pricingCtx);

	OrderDO_Ptr CreateOrder(OrderRequestDO& orderInfo);

	int OnMarketOrderUpdated(OrderDO& orderInfo);

	int Hedge(void);

	int Reset(void);

	void TradeByStrategy(const StrategyContractDO& strategyDO);

	void FillPosition(const ContractMap<double>& positionMap);

protected:
	PortfolioKey _portfolio;
	cuckoohash_map<ContractKey, bool, ContractKeyHash> _contractFlag;
	cuckoohash_map<ContractKey, double, ContractKeyHash> _contractPosition;
	MarketPositionContext _mktPosCtx;

	void SplitOrders(const OrderRequestDO& orderInfo, OrderRequestDO_Ptr& req1, OrderRequestDO_Ptr& req2);
private:

};

typedef std::shared_ptr<HedgeOrderManager> HedgeOrderManager_Ptr;

#endif