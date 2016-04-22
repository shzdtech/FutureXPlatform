/***********************************************************************
 * Module:  IOrderAPI.h
 * Author:  milk
 * Modified: 2016年1月16日 0:00:55
 * Purpose: Declaration of the class IOrderAPI
 ***********************************************************************/

#if !defined(__ordermanager_IOrderAPI_h)
#define __ordermanager_IOrderAPI_h

#include "../dataobject/OrderDO.h"

class IOrderAPI
{
public:
	virtual int CreateOrder(const OrderDO& orderInfo, OrderStatus& currStatus) = 0;
	virtual int CancelOrder(const OrderDO& orderInfo, OrderStatus& currStatus) = 0;

protected:
private:

};

#endif