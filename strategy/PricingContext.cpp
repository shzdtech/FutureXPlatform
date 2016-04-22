/***********************************************************************
 * Module:  PricingContext.cpp
 * Author:  milk
 * Modified: 2015年10月22日 19:24:41
 * Purpose: Implementation of the class PricingContext
 ***********************************************************************/

#include "PricingContext.h"

#include <set>
#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"


StrategyContractDOMap* PricingContext::GetStrategyMap()
{
	static StrategyContractDOMap _strategyContractDOMap;
	return &_strategyContractDOMap;
}

ContractDOMap* PricingContext::GetContractMap()
{
	static ContractDOMap _contractDOMap;
	return &_contractDOMap;
}

MarketDataDOMap* PricingContext::GetMarketDataDOMap()
{
	static MarketDataDOMap _marketDataDOMap;
	return &_marketDataDOMap;
}

PortfolioDOMap* PricingContext::GetPortfolioDOMap()
{
	static PortfolioDOMap _portfolioDOMap;
	return &_portfolioDOMap;
}