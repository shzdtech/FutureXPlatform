/***********************************************************************
 * Module:  OTCWorkerProcessor.h
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Declaration of the class OTCWorkerProcessor
 ***********************************************************************/

#if !defined(__OTCSERVER_OTCWorkerProcessor_h)
#define __OTCSERVER_OTCWorkerProcessor_h

#include "libcuckoo/cuckoohash_map.hh"
#include <list>
#include <future>
#include "../utility/cuckoohashmap_wrapper.h"
#include "../utility/lockfree_set.h"
#include "../ordermanager/OTCOrderManager.h"
#include "../dataobject/TypedefDO.h"
#include "../bizutility/ContractCache.h"
#include "../message/SessionContainer.h"
#include "OTCTradeWorkerProcessor.h"

#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCWorkerProcessor
{
public:
	OTCWorkerProcessor(const IPricingDataContext_Ptr& pricingCtx);
	~OTCWorkerProcessor();

	virtual void Initialize();

	virtual int LoadContractToCache(ProductType productType);

	virtual int LoadStrategyToCache(ProductType productType);

	virtual void LoadPortfolios();

	virtual int SubscribeStrategy(const StrategyContractDO& strategyDO);

	virtual void AddContractToMonitor(const ContractKey& contractId);

	virtual int SubscribePricingContracts(const UserContractKey& strategyKey, const StrategyPricingContract& strategyContract);

	virtual void AddMarketDataStrategyTrigger(const ContractKey & marketContract, const UserContractKey & strategyKey);

	virtual int UnsubscribePricingContracts(const UserContractKey & strategyKey, const StrategyPricingContract & strategyContract);

	virtual void TriggerUpdateByMarketData(const MarketDataDO& mdDO);

	virtual void TriggerPricingByStrategy(const StrategyContractDO & strategyDO);

	virtual bool TriggerTradingDeskParams(const StrategyContractDO& strategyDO);

	virtual void TriggerOTCPricing(const StrategyContractDO& strategyDO, bool findInCache);

	virtual void TriggerOTCTradingByStrategy(const StrategyContractDO & strategyDO);

	virtual void RegisterPricingListener(const ContractKey& contractId,	const IMessageSession_Ptr& sessionPtr);
	virtual void UnregisterPricingListener(const ContractKey& contractId, const IMessageSession_Ptr& sessionPtr);

	virtual void RegisterTradingDeskListener(const UserContractKey & userContractId, const IMessageSession_Ptr& sessionPtr);
	virtual void UnregisterTradingDeskListener(const UserContractKey & userContractId, const IMessageSession_Ptr& sessionPtr);

	virtual InstrumentCache& GetInstrumentCache();
	virtual const std::set<ProductType>& GetStrategyProductTypes() const;

	virtual int SubscribeMarketData(const ContractKey& contractId) = 0;
	virtual ProductType GetContractProductType() const = 0;

	

	virtual OTCTradeWorkerProcessor* GetOTCTradeWorkerProcessor() = 0;

	IPricingDataContext_Ptr& PricingDataContext();

protected:
	std::set<ProductType> _strategyProductTypes;

	cuckoohash_map<ContractKey, std::shared_ptr<lockfree_set<UserContractKey, UserContractKeyHash>>, ContractKeyHash> _baseContractStrategyMap;
	SessionContainer_Ptr<ContractKey, ContractKeyHash> _pricingNotifers;
	SessionContainer_Ptr<UserContractKey, UserContractKeyHash> _tradingDeskNotifers;
	SessionContainer_Ptr<uint64_t> _otcOrderNotifers;
	IPricingDataContext_Ptr _pricingCtx;

private:

};

#endif