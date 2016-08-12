#pragma once
#include "../dataobject/PricingDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TemplateDO.h"
#include "../utility/commonconst.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT OptionPricingCache
{
public:
	static void AddOption(const ContractKey& key, double volatility, double inputPrice, double outputPrice);

	static bool FindOption(const ContractKey& key, double volatility, double inputPrice, double& outputPrice);

	static void Clear(const ContractKey& key);

	static void Clear(void);
};

