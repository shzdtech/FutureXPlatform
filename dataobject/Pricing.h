#if !defined(__dataobject_Pricing_h)
#define __dataobject_Pricing_h

struct Pricing
{
	double Price = 0;
	int Volume = 0;
};

struct OptionPricing : public Pricing
{
	double Delta;
	double Vega;
	double Gamma;
	double Theta;
	double Volatility;
};

#endif