#include "OptionPricingCache.h"
#include "StaticInitializer.h"
#include "../utility/epsdouble.h"
#include "../utility/autofillmap.h"

static ContractMap<autofillmap<epsdouble, autofillmap<epsdouble, double>>> _optionCache;

void OptionPricingCache::AddOption(const ContractKey& key, double volatility, double inputPrice, double outputPrice)
{
	_optionCache.getorfill(key).getorfill(volatility).emplace(inputPrice, outputPrice);
}

bool OptionPricingCache::FindOption(const ContractKey& key, double volatility, double inputPrice, double& outputPrice)
{
	bool ret = false;

	auto it = _optionCache.find(key);
	if (it != _optionCache.end())
	{
		auto vit = it->second.find(volatility);
		if (vit != it->second.end())
		{
			auto pit = vit->second.find(inputPrice);
			if (pit != vit->second.end())
			{
				outputPrice = pit->second;
				ret = true;
			}
		}
	}

	return ret;
}

void OptionPricingCache::Clear(const ContractKey& key)
{
	_optionCache.erase(key);
}

void OptionPricingCache::Clear(void)
{
	_optionCache.clear();
}
