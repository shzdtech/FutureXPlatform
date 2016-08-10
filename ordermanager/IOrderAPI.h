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
	virtual OrderDO_Ptr CreateOrder(OrderRequestDO& orderInfo) = 0;
	virtual OrderDO_Ptr CancelOrder(OrderRequestDO& orderInfo) = 0;

protected:
private:

};

#endif