/***********************************************************************
 * Module:  CTPOTCTradeWorkerProcessorBase.h
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Declaration of the class CTPOTCTradeWorkerProcessorBase
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCTradeWorkerProcessorBase_h)
#define __CTPOTC_CTPOTCTradeWorkerProcessorBase_h

#include "../ordermanager/OTCOrderManager.h"
#include "../ordermanager/AutoOrderManager.h"
#include "../ordermanager/HedgeOrderManager.h"
#include "../ordermanager/UserTradeContext.h"
#include "../ordermanager/UserOrderContext.h"
#include "../message/SessionContainer.h"
#include "../dataobject/StrategyContractDO.h"
#include "../OTCServer/OTCTradeWorkerProcessor.h"
#include "../pricingengine/PricingDataContext.h"
#include "../utility/lockfree_set.h"
#include "CTPOTCTradeProcessor.h"

#include "ctpotc_export.h"

class CTPOTCWorkerProcessor;

class CTP_OTC_CLASS_EXPORT CTPOTCTradeWorkerProcessorBase : public OTCTradeWorkerProcessor
{
public:
	CTPOTCTradeWorkerProcessorBase(IServerContext* pServerCtx, const IPricingDataContext_Ptr& pricingDataCtx, const IUserPositionContext_Ptr& positionCtx);
	~CTPOTCTradeWorkerProcessorBase();

	virtual bool Dispose(void);
	virtual OTCOrderManager& GetOTCOrderManager(void);
	virtual AutoOrderManager& GetAutoOrderManager(void);
	virtual HedgeOrderManager& GetHedgeOrderManager(void);

	virtual void OnNewManualTrade(const TradeRecordDO & tradeDO);

	virtual void TriggerHedgeOrderUpdating(const PortfolioKey & portfolioKey, const IOrderAPI_Ptr& orderAPI_ptr);
	virtual void TriggerAutoOrderUpdating(const UserContractKey& strategyKey, const IOrderAPI_Ptr& orderAPI_ptr);

	std::shared_ptr<CTPOTCWorkerProcessor> GetMDWorkerProcessor();
	void SetMDWorkerProcessor(const std::shared_ptr<CTPOTCWorkerProcessor>& workerProcessor);

	virtual SessionContainer_Ptr<std::string>& GetUserSessionContainer(void) = 0;
	virtual IUserPositionContext_Ptr& GetUserPositionContext() = 0;
	virtual UserTradeContext& GetUserTradeContext() = 0;
	virtual UserOrderContext& GetExchangeOrderContext(void) = 0;

protected:
	virtual bool& Closing(void) = 0;

protected:
	std::shared_ptr<CTPOTCWorkerProcessor> _mdWorkerProcessor;

	OTCOrderManager _otcOrderMgr;
	AutoOrderManager _autoOrderMgr;
	HedgeOrderManager _hedgeOrderMgr;

	lockfree_set<std::pair<UserContractKey, IOrderAPI_Ptr>, pairhash_first<UserContractKey, IOrderAPI_Ptr, UserContractKeyHash>> _autoOrderQueue;
	lockfree_set<std::pair<PortfolioKey, IOrderAPI_Ptr>, pairhash_first<PortfolioKey, IOrderAPI_Ptr, PortfolioKeyHash>> _hedgeOrderQueue;
	lockfree_set<std::pair<PortfolioKey, IOrderAPI_Ptr>, pairhash_first<PortfolioKey, IOrderAPI_Ptr, PortfolioKeyHash>> _waitingHedgeQueue;

	std::future<void> _autoOrderWorker;
	std::future<void> _hedgeOrderWorker;

	void AutoOrderWorker();
	void HedgeOrderWorker();
};

typedef std::shared_ptr<CTPOTCTradeWorkerProcessorBase> CTPOTCTradeWorkerProcessorBase_Ptr;

#endif