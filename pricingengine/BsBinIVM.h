#if !defined(__pricingengine_BsBinIVM_h)
#define __pricingengine_BsBinIVM_h

#include "BlackScholesIVM.h"

class BsBinIVM : public BlackScholesIVM
{
public:
	virtual const std::string & Name(void) const;

	virtual double CaclImpliedVolatility(
		double marketOptionPrice,
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