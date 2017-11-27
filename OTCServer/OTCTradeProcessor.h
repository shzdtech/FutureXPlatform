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
#include "../ordermanager/UserOrderContext.h"
#include "../message/SessionContainer.h"
#include "../dataobject/StrategyContractDO.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCTradeProcessor : public IOrderAPI, public IOrderUpdatedEvent
{
public:
	virtual OrderDO_Ptr OTCNewOrder(OrderRequestDO& orderReq);
	virtual OrderDO_Ptr OTCCancelOrder(OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelAutoOrder(const UserContractKey& userContractKey);
	virtual OrderDO_Ptr CancelHedgeOrder(const PortfolioKey& portfolioKey);

	virtual OTCOrderManager& GetOTCOrderManager(void) = 0;
	virtual AutoOrderManager& GetAutoOrderManager(void) = 0;
	virtual HedgeOrderManager& GetHedgeOrderManager(void) = 0;

	virtual bool Dispose(void);

private:


};

typedef std::shared_ptr<OTCTradeProcessor> OTCTradeProcessor_Ptr;
#endif