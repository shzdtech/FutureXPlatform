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
	virtual int OnOrderUpdated(OrderDO& orderInfo) = 0;
};

class IOrderListener
{
public:
	std::function<void(OrderDO& orderDO)> OrderStatusUpdated = nullptr;
};

class IOrderManager : public IOrderUpdatedEvent
{
public:
	virtual int Reset() = 0;
	virtual OrderDO_Ptr FindOrder(uint64_t orderID) = 0;
	virtual int CreateOrder(OrderDO& orderInfo) = 0;
	virtual int CancelOrder(OrderDO& orderInfo) = 0;
	virtual int RejectOrder(OrderDO& orderInfo) = 0;
	virtual OrderDOVec_Ptr UpdateOrderByStrategy(
		const StrategyContractDO& strategyDO) = 0;

protected:
private:

};

typedef std::shared_ptr<IOrderManager> IOrderManager_Ptr;

typedef std::shared_ptr<IOrderListener> IOrderListener_Ptr;

#endif