#if !defined(__pricingengine_BsBinPricingAlgorithm_h)
#define __pricingengine_BsBinPricingAlgorithm_h

#include "BlackScholesPricingAlgorithm.h"

using namespace QuantLib;

class BsBinPricingAlgorithm : public BlackScholesPricingAlgorithm
{
public:
	virtual const std::string& Name(void) const;

	virtual std::shared_ptr<VanillaOption> ComputeOptionPrice(
		double underlyingPrice,
		double strikePrice,
		double volatility,
		double riskFreeRate,
		double dividendYield,
		ContractType contractType,
		const DateType& tradingDate,
		const DateType& maturityDate);
};

#endif

