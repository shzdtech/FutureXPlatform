/***********************************************************************
 * Module:  PricingContext.cpp
 * Author:  milk
 * Modified: 2015年10月22日 19:24:41
 * Purpose: Implementation of the class PricingContext
 ***********************************************************************/

#include "PricingDataContext.h"

#include <set>
#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"


StrategyContractDOMap* PricingDataContext::GetStrategyMap()
{
	return &_strategyContractDOMap;
}

ContractDOMap* PricingDataContext::GetContractMap()
{
	return &_contractDOMap;
}

MarketDataDOMap* PricingDataContext::GetMarketDataDOMap()
{
	return &_marketDataDOMap;
}

PortfolioDOMap* PricingDataContext::GetPortfolioDOMap()
{
	return &_portfolioDOMap;
}

InstrumentDOMap * PricingDataContext::GetInstrumentDOMap()
{
	return &_instrumentDOMap;
}
