#if !defined(__pricingengine_OptionParams_h)
#define __pricingengine_OptionParams_h

#include <string>
#include "../dataobject/ParamsBase.h"

class OptionParams : public ParamsBase
{
public:
	static constexpr double DEFAULT_VOLATILITY() { return 0.2; }
	double askVolatility = DEFAULT_VOLATILITY();
	double bidVolatility = DEFAULT_VOLATILITY();
	double midVolatility = DEFAULT_VOLATILITY();
	double riskFreeRate = 0;
	double dividend = 0;

public:
	static const std::string dividend_name;
	static const std::string riskFreeRate_name;
};

#endif