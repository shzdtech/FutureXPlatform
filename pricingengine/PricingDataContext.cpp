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

ContractParamDOMap* PricingDataContext::GetContractParamMap()
{
	return &_contractDOMap;
}

MarketDataDOMap* PricingDataContext::GetMarketDataMap()
{
	return &_marketDataDOMap;
}

PortfolioDOMap* PricingDataContext::GetPortfolioMap()
{
	return &_portfolioDOMap;
}

InstrumentDOMap * PricingDataContext::GetInstrumentDOMap()
{
	return &_instrumentDOMap;
}
