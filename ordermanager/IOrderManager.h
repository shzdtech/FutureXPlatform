/***********************************************************************
 * Module:  IOrderManager.h
 * Author:  milk
 * Modified: 2015年10月22日 23:35:55
 * Purpose: Declaration of the class IOrderManager
 ***********************************************************************/

#if !defined(__ordermanager_IOrderManager_h)
#define __ordermanager_IOrderManager_h

#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/OrderDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/TemplateDO.h"
#include <functional>

class IOrderUpdatedEvent
{
public:
	virtual void OnTraded(const TradeRecordDO_Ptr& tradeDO) = 0;
};

class IOrderManager
{
public:
	IOrderManager(IOrderUpdatedEvent* listener = nullptr) : _listener(listener){}
	virtual int Reset() = 0;
	virtual OrderDO_Ptr FindOrder(uint64_t orderID) = 0;
	virtual OrderDO_Ptr CreateOrder(OrderRequestDO& orderInfo) = 0;
	virtual OrderDO_Ptr CancelOrder(OrderRequestDO& orderInfo) = 0;
	virtual OrderDO_Ptr RejectOrder(OrderRequestDO& orderInfo) = 0;
	virtual void TradeByStrategy(const StrategyContractDO& strategyDO) = 0;

public:
	virtual int OnMarketOrderUpdated(OrderDO& orderInfo) = 0;

protected:
	IOrderUpdatedEvent* _listener;

private:

};

typedef std::shared_ptr<IOrderManager> IOrderManager_Ptr;

#endif