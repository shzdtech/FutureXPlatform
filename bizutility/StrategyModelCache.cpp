#include "StrategyModelCache.h"
#include "../utility/autofillmap.h"

static autofillmap<ModelKey, ModelParamsDO_Ptr> _modelCache;

void StrategyModelCache::AddModel(const ModelParamsDO_Ptr & modelptr)
{
	_modelCache.emplace(*modelptr, modelptr);
}

ModelParamsDO_Ptr StrategyModelCache::FindModel(const ModelKey & key)
{
	ModelParamsDO_Ptr ret;
	_modelCache.tryfind(key, ret);
	return ret;
}

void StrategyModelCache::Remove(const ModelKey & key)
{
	_modelCache.erase(key);
}

void StrategyModelCache::Clear(void)
{
	_modelCache.clear();
}
