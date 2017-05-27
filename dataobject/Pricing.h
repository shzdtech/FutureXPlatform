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
	double Volatility = 0;
	virtual void Clear()
	{
	}
};

#endif