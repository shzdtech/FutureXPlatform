/***********************************************************************
 * Module:  OTCTradeWorkerProcessor.h
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Declaration of the class OTCTradeWorkerProcessor
 ***********************************************************************/

#if !defined(__OTCSERVER_OTCTradeWorkerProcessor_h)
#define __OTCSERVER_OTCTradeWorkerProcessor_h

#include "../ordermanager/OTCOrderManager.h"
#include "../ordermanager/AutoOrderManager.h"
#include "../ordermanager/HedgeOrderManager.h"
#include "../ordermanager/UserOrderContext.h"
#include "../message/SessionContainer.h"
#include "../dataobject/StrategyContractDO.h"
#include "OTCTradeProcessor.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCTradeWorkerProcessor : public OTCTradeProcessor
{
public:
	OTCTradeWorkerProcessor(const IPricingDataContext_Ptr& pricingCtx);

	virtual void TriggerHedgeOrderUpdating(const PortfolioKey& portfolioKey, const IOrderAPI_Ptr& orderAPI_ptr = nullptr) = 0;
	virtual void TriggerAutoOrderUpdating(const UserContractKey& strategyKey, const IOrderAPI_Ptr& orderAPI_ptr = nullptr) = 0;

	virtual void TriggerOTCOrderUpdating(const StrategyContractDO& strategyDO);

	virtual UserOrderContext& GetExchangeOrderContext(void) = 0;
	virtual IPricingDataContext_Ptr& PricingDataContext(void);

protected:
	IPricingDataContext_Ptr _pricingCtx;

private:


};

typedef std::shared_ptr<OTCTradeWorkerProcessor> OTCTradeWorkerProcessor_Ptr;
#endif