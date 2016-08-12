#pragma once
#include "InstrumentCache.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT ContractCache
{
public:
	static InstrumentCache& Get(ProductType productType);
};

