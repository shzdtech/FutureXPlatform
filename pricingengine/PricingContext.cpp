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
	return &_strategyContractDOMap;
}

ContractDOMap* PricingContext::GetContractMap()
{
	return &_contractDOMap;
}

MarketDataDOMap* PricingContext::GetMarketDataDOMap()
{
	return &_marketDataDOMap;
}

PortfolioDOMap* PricingContext::GetPortfolioDOMap()
{
	return &_portfolioDOMap;
}

InstrumentDOMap * PricingContext::GetInstrumentDOMap()
{
	return &_instrumentDOMap;
}

PricingContext * PricingContext::Instance()
{
	static PricingContext* instance = new PricingContext();
	return instance;
}
