/***********************************************************************
 * Module:  BlackScholesPricingAlgorithm.h
 * Author:  milk
 * Modified: 2016年5月9日 22:57:32
 * Purpose: Declaration of the class BlackScholesPricingAlgorithm
 ***********************************************************************/

#if !defined(__pricingengine_BlackScholesPricingAlgorithm_h)
#define __pricingengine_BlackScholesPricingAlgorithm_h

#include "../dataobject/Pricing.h"
#include "IPricingAlgorithm.h"
#include "OptionParams.h"


class BlackScholesPricingAlgorithm : public IPricingAlgorithm
{
public:
	virtual const std::string& Name(void) const;
	virtual IPricingDO_Ptr Compute(
		const void* pInputObject,
		const StrategyContractDO& sdo,
		IPricingDataContext& priceCtx,
		const param_vector* params);
	virtual const std::map<std::string, double>& DefaultParams(void) const;
	virtual std::shared_ptr<void> ParseParams(const std::map<std::string, double>& modelParams);

	void ComputeOptionPrice(
		double underlyingPrice,
		double strikePrice,
		double volatility,
		double riskFreeRate,
		double dividendYield,
		ContractType contractType,
		const DateType& tradingDate,
		const DateType& maturityDate,
		OptionPricing & option);

private:

};

#endif