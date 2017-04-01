/***********************************************************************
 * Module:  PricingContext.h
 * Author:  milk
 * Modified: 2015年10月22日 19:24:41
 * Purpose: Declaration of the class PricingContext
 ***********************************************************************/

#if !defined(__pricingengine__PricingDataContext_h)
#define __pricingengine__PricingDataContext_h

#include <memory>
#include "IPricingDataContext.h"

#include "pricingengine_exp.h"

class PRICINGENGINE_CLASS_EXPORTS PricingDataContext : public IPricingDataContext
{
public:
	PricingDataContext();
	StrategyContractDOMap* GetStrategyMap();
	UserStrategyMap* GetUserStrategyMap();
	ContractParamDOMap* GetContractParamMap();
	MarketDataDOMap* GetMarketDataMap();
	PortfolioDOMap* GetPortfolioMap();
	PricingDataDOMap* GetPricingDataDOMap();

protected:

private:
	StrategyContractDOMap _strategyContractDOMap;
	UserStrategyMap _userStrategyMap;
	ContractParamDOMap _contractDOMap;
	MarketDataDOMap _marketDataDOMap;
	PortfolioDOMap _portfolioDOMap;
	PricingDataDOMap _pricingDataDOMap;
};

#endif