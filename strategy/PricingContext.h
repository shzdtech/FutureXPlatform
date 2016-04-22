/***********************************************************************
 * Module:  PricingContext.h
 * Author:  milk
 * Modified: 2015年10月22日 19:24:41
 * Purpose: Declaration of the class PricingContext
 ***********************************************************************/

#if !defined(__strategy__PricingContext_h)
#define __strategy__PricingContext_h

#include <memory>
#include "../message/AppContext.h"
#include "../message/IMessageProcessor.h"

#include "../dataobject/TypedefDO.h"
#include "strategy_exp.h"

class STRATEGY_CLASS_EXPORTS PricingContext
{
public:
	static StrategyContractDOMap* GetStrategyMap();
	static ContractDOMap* GetContractMap();
	static MarketDataDOMap* GetMarketDataDOMap();
	static PortfolioDOMap* GetPortfolioDOMap();

protected:
	

private:

};

#endif