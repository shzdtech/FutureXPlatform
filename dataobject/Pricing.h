#if !defined(__dataobject_Pricing_h)
#define __dataobject_Pricing_h

class Pricing
{
public:
	double Price = 0;
	int Volume = 0;
	virtual void Clear()
	{
		Price = 0;
		Volume = 0;
	}
};

class OptionPricing : public Pricing
{
public:
	double Delta = 0;
	double Vega = 0;
	double Gamma = 0;
	double Theta = 0;
	double Volatility = 0;
	virtual void Clear()
	{
	}
};

#endif