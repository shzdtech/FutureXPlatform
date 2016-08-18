#pragma once
#include "../dataobject/PricingDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TemplateDO.h"
#include "../utility/commonconst.h"
#include "../utility/epsdouble.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT OptionPricingCache
{
public:
	static std::pair<epsdouble, double>& FindOption(const ContractKey& key, double inputPrice);

	static void Clear(const ContractKey& key);

	static void Clear(void);
};

