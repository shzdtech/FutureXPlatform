/***********************************************************************
 * Module:  OrderManager.cpp
 * Author:  milk
 * Modified: 2016年1月16日 16:05:09
 * Purpose: Implementation of the class OrderManager
 ***********************************************************************/

#include "OrderManager.h"

////////////////////////////////////////////////////////////////////////
// Name:       OrderManager::OrderManager(IOrderAPI* pOrderAPI)
// Purpose:    Implementation of OrderManager::OrderManager()
// Parameters:
// - pOrderAPI
// Return:     
////////////////////////////////////////////////////////////////////////

OrderManager::OrderManager(IOrderAPI* pOrderAPI, const IPricingDataContext_Ptr& pricingCtx)
{
	_pOrderAPI = pOrderAPI;
	_pricingCtx = pricingCtx;
}

////////////////////////////////////////////////////////////////////////
// Name:       OrderManager::~OrderManager()
// Purpose:    Implementation of OrderManager::~OrderManager()
// Return:     
////////////////////////////////////////////////////////////////////////

OrderManager::~OrderManager()
{
   // TODO : implement
}

OrderDO_Ptr OrderManager::FindOrder(uint64_t orderID)
{
	return _userOrderCtx.FindOrder(orderID);
}