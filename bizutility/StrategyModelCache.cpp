#include "StrategyModelCache.h"
#include "../utility/autofillmap.h"
#include "../databaseop/ModelParamsDAO.h"

static autofillmap<ModelKey, ModelParamsDO_Ptr> _modelCache;
StrategyModelCache::static_initializer StrategyModelCache::_static_init;

ModelParamsDO_Ptr StrategyModelCache::FindModel(const ModelKey & key)
{
	ModelParamsDO_Ptr ret;
	_modelCache.tryfind(key, ret);
	return ret;
}

ModelParamsDO_Ptr StrategyModelCache::FindOrRetrieveModel(const ModelKey & key)
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

vector_ptr<ModelParamsDO_Ptr> StrategyModelCache::FindModelsByUser(const std::string& userId)
{
	vector_ptr<ModelParamsDO_Ptr> ret = std::make_shared<std::vector<ModelParamsDO_Ptr>>();

	for (auto it = _modelCache.begin();;it++)
	{
		it = std::find_if(it, _modelCache.end(),
			[&userId](std::pair<const ModelKey, ModelParamsDO_Ptr>& pair) { return pair.first.UserID() == userId; });
		if (it == _modelCache.end())
			break;

		ret->push_back(it->second);
	}

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

StrategyModelCache::static_initializer::static_initializer()
{
	autofillmap<ModelKey, ModelParamsDO_Ptr> modelMap;
	ModelParamsDAO::FindAllModels(modelMap);

	for (auto& pair : modelMap)
	{
		_modelCache.emplace(pair.first, std::make_shared<ModelParamsDO>(std::move(*pair.second)));
	}
}
