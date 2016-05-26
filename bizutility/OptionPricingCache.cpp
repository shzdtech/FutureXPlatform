#include "OptionPricingCache.h"
#include "StaticInitializer.h"
#include "../utility/epsdouble.h"
#include "../utility/autofillmap.h"

static UserContractMap<autofillmap<epsdouble, autofillmap<epsdouble, PricingDO>>> _optionPricingCache;

void OptionPricingCache::Add(const StrategyContractDO& key, double price, const PricingDO & pricingDO)
{
	_optionPricingCache.getorfill(key).getorfill(key.Volatility).emplace(price, pricingDO);
}

PricingDO* OptionPricingCache::Find(const StrategyContractDO& key, double price)
{
	PricingDO* ret = nullptr;

	auto it = _optionPricingCache.find(key);
	if (it != _optionPricingCache.end())
	{
		auto vit = it->second.find(key.Volatility);
		if (vit != it->second.end())
		{
			auto pit = vit->second.find(price);
			if (pit != vit->second.end())
				ret = &pit->second;
		}
	}

	return ret;
}

void OptionPricingCache::Clear(const UserContractKey& key)
{
	_optionPricingCache.erase(key);
}

void OptionPricingCache::Clear(void)
{
	_optionPricingCache.clear();
}
