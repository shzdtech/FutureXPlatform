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
#include "../message/SysParam.h"
#include "../message/AppContext.h"


bool OTCTradeProcessor::Dispose(void)
{
	auto pMap = GetPricingContext()->GetStrategyMap();
	for (auto& it : *pMap)
	{
		auto& stragety = it.second;
		stragety.Enabled = false;
		stragety.Trading = false;
	}

	GetOTCOrderManager()->Reset();
	GetAutoOrderManager()->Reset();

	return true;
}


OrderDOVec_Ptr OTCTradeProcessor::TriggerHedgeOrderUpdating(const StrategyContractDO& strategyDO)
{
	return GetAutoOrderManager()->UpdateOrderByStrategy(strategyDO);
}

OrderDOVec_Ptr OTCTradeProcessor::TriggerOTCOrderUpdating(const StrategyContractDO& strategyDO)
{
	return GetOTCOrderManager()->UpdateOrderByStrategy(strategyDO);
}

OrderDO_Ptr OTCTradeProcessor::OTCNewOrder(OrderRequestDO& orderReq)
{
	return GetOTCOrderManager()->CreateOrder(orderReq);
}

OrderDO_Ptr OTCTradeProcessor::OTCCancelOrder(OrderRequestDO& orderReq)
{
	return GetOTCOrderManager()->CancelOrder(orderReq);
}

OrderDO_Ptr OTCTradeProcessor::CancelHedgeOrder(const UserContractKey& userContractKey)
{
	return GetAutoOrderManager()->CancelOrder(OrderRequestDO(userContractKey));
}