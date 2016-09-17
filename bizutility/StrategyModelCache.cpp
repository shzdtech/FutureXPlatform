#include "StrategyModelCache.h"
#include "../utility/autofillmap.h"
#include "../databaseop/ModelParamsDAO.h"

static autofillmap<ModelKey, ModelParamsDO_Ptr> _modelCache;
static StrategyModelCache::static_initializer _static_initializer;

ModelParamsDO_Ptr StrategyModelCache::FindModel(const ModelKey & key)
{
	ModelParamsDO_Ptr ret;
	_modelCache.tryfind(key, ret);
	return ret;
}

ModelParamsDO_Ptr StrategyModelCache::FindOrCreateModel(const ModelKey & key)
{
	ModelParamsDO_Ptr ret = FindModel(key);

	if (!ret)
	{
		if (ret = ModelParamsDAO::FindUserModel(key.UserID(), key.InstanceName))
		{
			ret = std::make_shared<ModelParamsDO>(std::move(*ret));
			_modelCache.emplace(key, ret);
		}
	}

	return ret;
}

const std::map<ModelKey, ModelParamsDO_Ptr>& StrategyModelCache::ModelCache()
{
	return _modelCache;
}

void StrategyModelCache::Remove(const ModelKey & key)
{
	_modelCache.erase(key);
}

void StrategyModelCache::Clear(void)
{
	_modelCache.clear();
}

StrategyModelCache::static_initializer::static_initializer()
{
	autofillmap<ModelKey, ModelParamsDO_Ptr> modelMap;
	ModelParamsDAO::FindAllModels(modelMap);

	for (auto& pair : modelMap)
	{
		_modelCache.emplace(pair.first, std::make_shared<ModelParamsDO>(std::move(*pair.second)));
	}
}
