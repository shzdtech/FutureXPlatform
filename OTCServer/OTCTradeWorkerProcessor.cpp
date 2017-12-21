/***********************************************************************
 * Module:  OTCTradeWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Implementation of the class OTCTradeWorkerProcessor
 ***********************************************************************/
#include "OTCTradeWorkerProcessor.h"
#include "../common/Attribute_Key.h"

#include "../message/message_macro.h"
#include "../message/DefMessageID.h"
#include "../systemsettings/AppContext.h"



OTCTradeWorkerProcessor::OTCTradeWorkerProcessor(const IPricingDataContext_Ptr& pricingCtx) :
	_pricingCtx(pricingCtx)
{
}

void OTCTradeWorkerProcessor::TriggerOTCOrderUpdating(const StrategyContractDO& strategyDO)
{
	GetOTCOrderManager().TradeByStrategy(strategyDO, this);
}

IPricingDataContext_Ptr& OTCTradeWorkerProcessor::PricingDataContext(void)
{
	return _pricingCtx;
}
