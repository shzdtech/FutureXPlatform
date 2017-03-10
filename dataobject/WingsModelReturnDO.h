#pragma once
#include "PricingDO.h"

class WingsModelReturnDO : public OptionPricingDO
{
public:
	double f_syn = 0;
	double x = 0;
	double vol_curr = 0;
	double vol_curr_offset = 0;
	double slope_curr = 0;
	double slope_curr_offset = 0;
	double x0 = 0;
	double x0_offset = 0;
	double x1 = 0;
	double x1_offset = 0;
	double x2 = 0;
	double x2_offset = 0;
	double x3 = 0;
	double x3_offset = 0;
};

typedef std::shared_ptr<WingsModelReturnDO> WingsModelReturnDO_Ptr;