#pragma once
#include "../dataobject/PricingDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TemplateDO.h"
#include "../utility/commonconst.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT OptionPricingCache
{
public:
	static void AddCallOption(const StrategyContractDO& key, double inputPrice, double outputPrice);

	static void AddPutOption(const StrategyContractDO& key, double inputPrice, double outputPrice);

	static bool FindCallOption(const StrategyContractDO& key, double inputPrice, double& outputPrice);

	static bool FindPutOption(const StrategyContractDO& key, double inputPrice, double& outputPrice);

	static void Clear(const UserContractKey& key);

	static void Clear(void);
};

