/***********************************************************************
 * Module:  OrderManager.h
 * Author:  milk
 * Modified: 2016年1月16日 16:05:09
 * Purpose: Declaration of the class OrderManager
 ***********************************************************************/

#if !defined(__ordermanager_OrderManager_h)
#define __ordermanager_OrderManager_h

#include "IOrderManager.h"
#include "IOrderAPI.h"
#include "ContractOrderContext.h"
#include "../pricingengine/IPricingDataContext.h"

#include "ordermgr_export.h"

class ORDERMGR_CLASS_EXPORT OrderManager : public IOrderManager
{
public:
   OrderManager(IOrderAPI* pOrderAPI, const IPricingDataContext_Ptr& pricingCtx, IOrderUpdatedEvent* listener = nullptr);
   ~OrderManager();

   OrderDO_Ptr FindOrder(uint64_t orderID);

protected:
   IOrderAPI* _pOrderAPI;
   IPricingDataContext_Ptr _pricingCtx;
   ContractOrderContext _contractOrderCtx;

private:

};

#endif