/***********************************************************************
 * Module:  OTCTradeProcessor.h
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Declaration of the class OTCTradeProcessor
 ***********************************************************************/

#if !defined(__OTCSERVER_OTCTradeProcessor_h)
#define __OTCSERVER_OTCTradeProcessor_h

#include "../ordermanager/OTCOrderManager.h"
#include "../ordermanager/AutoOrderManager.h"
#include "../ordermanager/HedgeOrderManager.h"
#include "../message/SessionContainer.h"
#include "../dataobject/StrategyContractDO.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCTradeProcessor : public IOrderAPI
{
public:
	virtual OrderDOVec_Ptr TriggerHedgeOrderUpdating(const StrategyContractDO& strategyDO);
	virtual OrderDOVec_Ptr TriggerOTCOrderUpdating(const StrategyContractDO& strategyDO);

	virtual OrderDO_Ptr OTCNewOrder(OrderRequestDO& orderReq);
	virtual OrderDO_Ptr OTCCancelOrder(OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelHedgeOrder(const UserContractKey& userContractKey);

	virtual IPricingDataContext* GetPricingContext(void) = 0;
	virtual OTCOrderManager* GetOTCOrderManager(void) = 0;
	virtual AutoOrderManager* GetAutoOrderManager(void) = 0;

	virtual bool Dispose(void);

protected:

private:


};

#endif