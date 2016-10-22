#if !defined(__pricingengine_OptionParams_h)
#define __pricingengine_OptionParams_h

#include <string>
#include "../dataobject/ParamsBase.h"

class OptionParams : public ParamsBase
{
public:
	double askVolatility = 0.2;
	double bidVolatility = 0.2;
	double riskFreeRate = 0;
	double dividend = 0;

public:
	static const std::string dividend_name;
	static const std::string riskFreeRate_name;
};

#endif