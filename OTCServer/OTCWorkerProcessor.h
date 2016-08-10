/***********************************************************************
 * Module:  OTCWorkerProcessor.h
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Declaration of the class OTCWorkerProcessor
 ***********************************************************************/

#if !defined(__OTCSERVER_OTCWorkerProcessor_h)
#define __OTCSERVER_OTCWorkerProcessor_h

#include <set>
#include <list>
#include "../ordermanager/OTCOrderManager.h"
#include "../dataobject/TypedefDO.h"
#include "../message/SessionContainer.h"
#include "OTCTradeProcessor.h"

#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCWorkerProcessor
{
public:
	OTCWorkerProcessor();
   ~OTCWorkerProcessor();

   virtual int LoginSystemUserIfNeed(void) = 0;
   virtual int RefreshStrategy(const StrategyContractDO& strategyDO);
   virtual void AddContractToMonitor(const ContractKey& contractId);
  
   virtual void TriggerPricing(const StrategyContractDO& strategyDO);
   virtual void TriggerUpdating(const MarketDataDO& mdDO);
   
   virtual void RegisterPricingListener(const ContractKey& contractId,
	   IMessageSession* pMsgSession);

   virtual void UnregisterPricingListener(const ContractKey& contractId,
	   IMessageSession* pMsgSession);

   virtual OTCTradeProcessor* GetOTCTradeProcessor() = 0;

protected:
	ContractMap<std::set<ContractKey >> _contract_strategy_map;
	SessionContainer_Ptr<ContractKey> _pricingNotifers;
	SessionContainer_Ptr<uint64_t> _otcOrderNotifers;

private:

};

#endif