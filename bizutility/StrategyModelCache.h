#pragma once
#include "../dataobject/ModelParamsDO.h"
#include "../dataobject/TypedefDO.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT StrategyModelCache
{
public:
	static void AddModel(const ModelParamsDO_Ptr& modelptr);

	static ModelParamsDO_Ptr FindModel(const ModelKey& key);

	static void Remove(const ModelKey& key);

	static void Clear(void);
};

