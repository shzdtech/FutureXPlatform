#include "OptionPricingCache.h"
#include "StaticInitializer.h"
#include "../utility/epsdouble.h"
#include "../utility/autofillmap.h"

static UserContractMap<autofillmap<epsdouble, autofillmap<epsdouble, double>>> _callOptionCache;
static UserContractMap<autofillmap<epsdouble, autofillmap<epsdouble, double>>> _putOptionCache;

void OptionPricingCache::AddCallOption(const StrategyContractDO& key, double inputPrice, double outputPrice)
{
	_callOptionCache.getorfill(key).getorfill(key.Volatility).emplace(inputPrice, outputPrice);
}

void OptionPricingCache::AddPutOption(const StrategyContractDO& key, double inputPrice, double outputPrice)
{
	_putOptionCache.getorfill(key).getorfill(key.Volatility).emplace(inputPrice, outputPrice);
}

bool OptionPricingCache::FindCallOption(const StrategyContractDO& key, double inputPrice, double& outputPrice)
{
	bool ret = false;

	auto it = _callOptionCache.find(key);
	if (it != _callOptionCache.end())
	{
		auto vit = it->second.find(key.Volatility);
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

bool OptionPricingCache::FindPutOption(const StrategyContractDO& key, double inputPrice, double& outputPrice)
{
	bool ret = false;

	auto it = _putOptionCache.find(key);
	if (it != _putOptionCache.end())
	{
		auto vit = it->second.find(key.Volatility);
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

void OptionPricingCache::Clear(const UserContractKey& key)
{
	_callOptionCache.erase(key);
	_putOptionCache.erase(key);
}

void OptionPricingCache::Clear(void)
{
	_callOptionCache.clear();
	_putOptionCache.clear();
}
