/***********************************************************************
 * Module:  BlackScholesPricingAlgorithm.h
 * Author:  milk
 * Modified: 2016年5月9日 22:57:32
 * Purpose: Declaration of the class BlackScholesPricingAlgorithm
 ***********************************************************************/

#if !defined(__pricingengine_BlackScholesIVM_h)
#define __pricingengine_BlackScholesIVM_h

#include "../dataobject/Pricing.h"
#include "IModelAlgorithm.h"
#include "OptionParams.h"

class BlackScholesIVM : public IModelAlgorithm
{
public:
	virtual const std::string& Name(void) const;
	virtual dataobj_ptr Compute(
		const void* pInputObject,
		const StrategyContractDO& sdo,
		IPricingDataContext& priceCtx,
		const param_vector* params);
	virtual const std::map<std::string, double>& DefaultParams(void) const;
	virtual std::shared_ptr<void> ParseParams(const std::map<std::string, double>& modelParams);

	static double ImpliedVolatility(
		double marketOptionPrice,
		double underlyingPrice,
		double strikePrice,
		double volatility,
		double riskFreeRate,
		double dividendYield,
		ContractType contractType,
		const DateType& tradingDate,
		const DateType& maturityDate);

private:

};

#endif