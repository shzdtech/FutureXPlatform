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
	StrategyContractDOMap* GetStrategyMap();
	ContractParamDOMap* GetContractParamMap();
	MarketDataDOMap* GetMarketDataMap();
	PortfolioDOMap* GetPortfolioMap();
	PricingDataDOMap* GetPricingDataDOMap();

protected:

private:
	StrategyContractDOMap _strategyContractDOMap;
	ContractParamDOMap _contractDOMap;
	MarketDataDOMap _marketDataDOMap;
	PortfolioDOMap _portfolioDOMap;
	PricingDataDOMap _pricingDataDOMap;
};

#endif