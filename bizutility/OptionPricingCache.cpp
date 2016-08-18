#include "OptionPricingCache.h"
#include "StaticInitializer.h"
#include "../utility/epsdouble.h"
#include "../utility/autofillmap.h"

static ContractMap<autofillmap<epsdouble, std::pair<epsdouble, double>>> _optionCache;


std::pair<epsdouble, double>& OptionPricingCache::FindOption(const ContractKey& key, double inputPrice)
{
	return _optionCache.getorfill(key).getorfill(inputPrice);
}

void OptionPricingCache::Clear(const ContractKey& key)
{
	_optionCache.erase(key);
}

void OptionPricingCache::Clear(void)
{
	_optionCache.clear();
}
