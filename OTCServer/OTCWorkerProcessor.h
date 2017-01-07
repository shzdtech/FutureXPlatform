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
#include <thread>
#include "../ordermanager/OTCOrderManager.h"
#include "../dataobject/TypedefDO.h"
#include "../bizutility/ContractCache.h"
#include "../message/SessionContainer.h"
#include "OTCTradeProcessor.h"

#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCWorkerProcessor
{
public:
	OTCWorkerProcessor(const IPricingDataContext_Ptr& pricingCtx);
	~OTCWorkerProcessor();

	virtual void Initialize();
	virtual int LoginSystemUserIfNeed(void) = 0;

	virtual int LoadContractToCache(ProductType productType);
	virtual int LoadStrategyToCache(ProductType productType);

	virtual int SubscribeStrategy(const StrategyContractDO& strategyDO);
	virtual void AddContractToMonitor(const ContractKey& contractId);

	virtual void TriggerOTCPricing(const StrategyContractDO& strategyDO);
	virtual void TriggerTadingDeskParams(const StrategyContractDO& strategyDO);
	virtual void TriggerUpdating(const MarketDataDO& mdDO);
	virtual void TriggerOTCUpdating(const StrategyContractDO& strategyDO);

	virtual void RegisterPricingListener(const ContractKey& contractId,
		const IMessageSession_Ptr& sessionPtr);
	virtual void UnregisterPricingListener(const ContractKey& contractId,
		const IMessageSession_Ptr& sessionPtr);

	virtual void RegisterTradingDeskListener(const ContractKey& contractId,
		const IMessageSession_Ptr& sessionPtr);
	virtual void UnregisterTradingDeskListener(const ContractKey& contractId,
		const IMessageSession_Ptr& sessionPtr);

	virtual InstrumentCache& GetInstrumentCache();

	virtual int SubscribeMarketData(const ContractKey& contractId) = 0;

	virtual ProductType GetContractProductType() const = 0;

	virtual const std::vector<ProductType>& GetStrategyProductTypes() const = 0;

	virtual OTCTradeProcessor* GetOTCTradeProcessor() = 0;

	IPricingDataContext_Ptr& PricingDataContext();

protected:
	cuckoohash_map<ContractKey, std::shared_ptr<std::set<ContractKey>>, ContractKeyHash> _baseContractStrategyMap;
	cuckoohash_map<ContractKey, bool, ContractKeyHash> _exchangeStrategySet;
	cuckoohash_map<ContractKey, bool, ContractKeyHash> _otcStrategySet;
	SessionContainer_Ptr<ContractKey, ContractKeyHash> _pricingNotifers;
	SessionContainer_Ptr<ContractKey, ContractKeyHash> _tradingDeskNotifers;
	SessionContainer_Ptr<uint64_t> _otcOrderNotifers;
	IPricingDataContext_Ptr _pricingCtx;

	bool _runingTradingDeskFlag;
	std::thread _tradingDeskWorderThread;

private:

};

#endif