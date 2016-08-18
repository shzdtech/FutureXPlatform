/***********************************************************************
 * Module:  OrderDAO.h
 * Author:  milk
 * Modified: 2015年10月16日 0:28:29
 * Purpose: Declaration of the class OrderDAO
 ***********************************************************************/

#if !defined(__databaseop_OrderDAO_h)
#define __databaseop_OrderDAO_h

#include "../dataobject/OrderDO.h"
#include "../dataobject/ContractKey.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/StrategyContractDO.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS OrderDAO
{
public:
	OrderDO_Ptr CreateOrder(const OrderDO& orderDO);

protected:
private:

};

#endif