/***********************************************************************
 * Module:  OTCTradeProcessor.cpp
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Implementation of the class OTCTradeProcessor
 ***********************************************************************/
#include "OTCTradeProcessor.h"
#include "../common/Attribute_Key.h"

#include "../message/message_macro.h"
#include "../message/DefMessageID.h"
#include "../systemsettings/AppContext.h"


bool OTCTradeProcessor::Dispose(void)
{
	return true;
}

OrderDO_Ptr OTCTradeProcessor::OTCNewOrder(OrderRequestDO& orderReq)
{
	return GetOTCOrderManager().CreateOrder(orderReq, this);
}

OrderDO_Ptr OTCTradeProcessor::OTCCancelOrder(OrderRequestDO& orderReq)
{
	return GetOTCOrderManager().CancelOrder(orderReq, this);
}

OrderDO_Ptr OTCTradeProcessor::CancelAutoOrder(const UserContractKey& userContractKey)
{
	return GetAutoOrderManager().CancelOrder(OrderRequestDO(userContractKey, ""), this);
}

OrderDO_Ptr OTCTradeProcessor::CancelHedgeOrder(const PortfolioKey& portfolioKey)
{
	return GetHedgeOrderManager().CancelOrder(OrderRequestDO(0, "", "", portfolioKey.UserID(), portfolioKey.PortfolioID()), this);
}