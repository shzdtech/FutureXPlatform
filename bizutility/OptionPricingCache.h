#pragma once
#include "../dataobject/PricingDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TemplateDO.h"
#include "../utility/commonconst.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT OptionPricingCache
{
public:
	static void Add(const StrategyContractDO& key, double price, const PricingDO& prcingDO);

	static PricingDO* Find(const StrategyContractDO& key, double price);

	static void Clear(const UserContractKey& key);

	static void Clear(void);
};

