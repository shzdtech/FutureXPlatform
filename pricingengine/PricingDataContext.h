/***********************************************************************
 * Module:  PricingContext.h
 * Author:  milk
 * Modified: 2015年10月22日 19:24:41
 * Purpose: Declaration of the class PricingContext
 ***********************************************************************/

#if !defined(__strategy__PricingDataContext_h)
#define __strategy__PricingDataContext_h

#include <memory>
#include "IPricingDataContext.h"

#include "pricingengine_exp.h"

class PRICINGENGINE_CLASS_EXPORTS PricingDataContext : IPricingDataContext
{
public:
	StrategyContractDOMap* GetStrategyMap();
	ContractDOMap* GetContractMap();
	MarketDataDOMap* GetMarketDataDOMap();
	PortfolioDOMap* GetPortfolioDOMap();
	InstrumentDOMap* GetInstrumentDOMap();

protected:

private:
	StrategyContractDOMap _strategyContractDOMap;
	ContractDOMap _contractDOMap;
	MarketDataDOMap _marketDataDOMap;
	PortfolioDOMap _portfolioDOMap;
	InstrumentDOMap _instrumentDOMap;

};

#endif