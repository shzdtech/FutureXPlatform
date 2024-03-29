/***********************************************************************
 * Module:  IPricingDataContext.h
 * Author:  milk
 * Modified: 2016年5月11日 18:18:26
 * Purpose: Declaration of the class IPricingDataContext
 ***********************************************************************/

#if !defined(__pricingengine_IPricingDataContext_h)
#define __pricingengine_IPricingDataContext_h

#include <memory>
#include "../dataobject/TypedefDO.h"

class IPricingDataContext
{
public:
	virtual StrategyContractDOMap* GetStrategyMap(void) = 0;
	virtual UserStrategyMap* GetUserStrategyMap(void) = 0;
	virtual ContractParamDOMap* GetContractParamMap(void) = 0;
	virtual MarketDataDOMap* GetMarketDataMap(void) = 0;
	virtual PortfolioDOMap* GetPortfolioMap(void) = 0;
	virtual PricingDataDOMap* GetPricingDataDOMap(void) = 0;

protected:
private:

};

typedef std::shared_ptr<IPricingDataContext> IPricingDataContext_Ptr;

#endif