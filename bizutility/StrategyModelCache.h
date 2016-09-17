#pragma once
#include "../dataobject/ModelParamsDO.h"
#include "../dataobject/TypedefDO.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT StrategyModelCache
{
public:
	static ModelParamsDO_Ptr FindModel(const ModelKey& key);

	static ModelParamsDO_Ptr FindOrCreateModel(const ModelKey & key);

	static const std::map<ModelKey, ModelParamsDO_Ptr>& ModelCache();

	static void Remove(const ModelKey& key);

	static void Clear(void);

	class static_initializer
	{
	public:
		static_initializer();
	};

private:
	static static_initializer _static_initializer;
};

