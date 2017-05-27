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
	GetOTCOrderManager().Reset();
	GetAutoOrderManager().Reset();
	GetHedgeOrderManager().Reset();

	return true;
}


OTCTradeProcessor::OTCTradeProcessor(const IPricingDataContext_Ptr& pricingCtx) :
	_pricingCtx(pricingCtx)
{
}

void OTCTradeProcessor::TriggerHedgeOrderUpdating(const PortfolioKey& portfolioKey)
{
	GetHedgeOrderManager().Hedge(portfolioKey);
}

void OTCTradeProcessor::TriggerAutoOrderUpdating(const StrategyContractDO& strategyDO)
{
	GetAutoOrderManager().TradeByStrategy(strategyDO);
}

void OTCTradeProcessor::TriggerOTCOrderUpdating(const StrategyContractDO& strategyDO)
{
	GetOTCOrderManager().TradeByStrategy(strategyDO);
}

OrderDO_Ptr OTCTradeProcessor::OTCNewOrder(OrderRequestDO& orderReq)
{
	return GetOTCOrderManager().CreateOrder(orderReq);
}

OrderDO_Ptr OTCTradeProcessor::OTCCancelOrder(OrderRequestDO& orderReq)
{
	return GetOTCOrderManager().CancelOrder(orderReq);
}

OrderDO_Ptr OTCTradeProcessor::CancelAutoOrder(const UserContractKey& userContractKey)
{
	return GetAutoOrderManager().CancelOrder(OrderRequestDO(userContractKey, ""));
}

OrderDO_Ptr OTCTradeProcessor::CancelHedgeOrder(const PortfolioKey& portfolioKey)
{
	return GetHedgeOrderManager().CancelOrder(OrderRequestDO(0, "", "", portfolioKey.UserID(), portfolioKey.PortfolioID()));
}

IPricingDataContext_Ptr& OTCTradeProcessor::PricingDataContext(void)
{
	return _pricingCtx;
}
