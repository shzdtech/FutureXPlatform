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


PricingDataContext::PricingDataContext()
	: _strategyContractDOMap(256), _marketDataDOMap(256), _pricingDataDOMap(256)
{
}

StrategyContractDOMap* PricingDataContext::GetStrategyMap()
{
	return &_strategyContractDOMap;
}

UserStrategyMap* PricingDataContext::GetUserStrategyMap()
{
	return &_userStrategyMap;
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

PricingDataDOMap * PricingDataContext::GetPricingDataDOMap()
{
	return &_pricingDataDOMap;
}
