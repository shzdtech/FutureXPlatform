/***********************************************************************
 * Module:  OTCOrderDAO.h
 * Author:  milk
 * Modified: 2015年10月14日 14:58:04
 * Purpose: Declaration of the class OrderDAO
 ***********************************************************************/

#if !defined(__databaseop_OTCOrderDAO_h)
#define __databaseop_OTCOrderDAO_h

#include "../dataobject/OrderDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/PricingDO.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS OTCOrderDAO
{
public:
	static OrderDO_Ptr CreateOrder(const OrderRequestDO& orderDO, const IPricingDO& pricingDO);
	static bool CancelOrder(const OrderRequestDO& orderDO, OrderStatusType& status);
	static bool AcceptOrder(const OrderRequestDO& orderDO, OrderStatusType& status);
	static bool RejectOrder(const OrderRequestDO& orderDO, OrderStatusType& status);
	static OrderDOVec_Ptr QueryTradingOrder(const ContractKey& contractKey);
	static OrderDOVec_Ptr QueryTodayOrder(const std::string& userId, const ContractKey& contractKey);

protected:
private:

};

#endif