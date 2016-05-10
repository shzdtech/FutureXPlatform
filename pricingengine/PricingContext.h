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
#include "../utility/singleton_templ.h"

#include "pricingengine_exp.h"

class PRICINGENGINE_CLASS_EXPORTS PricingContext
{
public:
	StrategyContractDOMap* GetStrategyMap();
	ContractDOMap* GetContractMap();
	MarketDataDOMap* GetMarketDataDOMap();
	PortfolioDOMap* GetPortfolioDOMap();
	InstrumentDOMap* GetInstrumentDOMap();

	static PricingContext* Instance();

protected:
	static PricingContext* _defaultInstance;

private:
	StrategyContractDOMap _strategyContractDOMap;
	ContractDOMap _contractDOMap;
	MarketDataDOMap _marketDataDOMap;
	PortfolioDOMap _portfolioDOMap;
	InstrumentDOMap _instrumentDOMap;

};

#endif