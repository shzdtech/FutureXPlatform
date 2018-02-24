#include "ModelParamsCache.h"
#include "../utility/autofillmap.h"
#include "../include/libcuckoo/cuckoohash_map.hh"
#include "../databaseop/ModelParamsDAO.h"

static cuckoohash_map<ModelKey, ModelParamsDO_Ptr, ModelKeyHash> _modelCache(256);
static cuckoohash_map<std::string, vector_ptr<ModelParamsDO_Ptr>> _userModelCache(256);
static cuckoohash_map<ModelKey, ModelParamsDO_Ptr, ModelKeyHash> _modelCacheTemp(4);

ModelParamsDO_Ptr ModelParamsCache::FindModel(const ModelKey & key)
{
	ModelParamsDO_Ptr ret;
	_modelCache.find(key, ret);
	return ret;
}

ModelParamsDO_Ptr ModelParamsCache::FindTempModel(const ModelKey & key)
{
	ModelParamsDO_Ptr ret;
	_modelCacheTemp.find(key, ret);
	return ret;
}

bool ModelParamsCache::InsertTempModel(const ModelParamsDO_Ptr& model)
{
	return _modelCacheTemp.insert(*model, model);
}

ModelParamsDO_Ptr ModelParamsCache::FindOrRetrieveModel(const ModelKey & key)
{
	ModelParamsDO_Ptr ret = FindModel(key);

	if (!ret)
	{
		if (ret = ModelParamsDAO::FindUserModel(key.UserID(), key.InstanceName))
		{
			ret = std::make_shared<ModelParamsDO>(std::move(*ret));
			if (_modelCache.insert(key, ret))
			{
				vector_ptr<ModelParamsDO_Ptr> model_vec;
				if (_userModelCache.find(key.UserID(), model_vec))
					model_vec->push_back(ret);
			}
		}
	}

	return ret;
}

vector_ptr<ModelParamsDO_Ptr> ModelParamsCache::FindModelsByUser(const std::string& userId)
{
	vector_ptr<ModelParamsDO_Ptr> ret;

	_userModelCache.find(userId, ret);

	return ret;
}

void ModelParamsCache::Remove(const ModelKey & key)
{
	if (_modelCache.erase(key))
	{
		vector_ptr<ModelParamsDO_Ptr> model_vec;
		if (_userModelCache.find(key.UserID(), model_vec))
		{
			for (auto it = model_vec->begin(); it != model_vec->end();)
			{
				if (it->get()->InstanceName == key.InstanceName)
					it = model_vec->erase(it);
				else
					it++;
			}
		}
	}
}

bool ModelParamsCache::RemoveTempModel(const ModelKey & key)
{
	return _modelCacheTemp.erase(key);
}

void ModelParamsCache::Clear(void)
{
	_modelCache.clear();
	_modelCacheTemp.clear();
	_userModelCache.clear();
}

void ModelParamsCache::Load(const std::string& userId)
{
	if (!_userModelCache.contains(userId))
	{
		autofillmap<ModelKey, ModelParamsDO_Ptr> modelMap;
		ModelParamsDAO::FindAllModels(userId, modelMap);

		auto model_vec = std::make_shared<std::vector<ModelParamsDO_Ptr>>();
		for (auto& pair : modelMap)
		{
			auto model_ptr = std::make_shared<ModelParamsDO>(std::move(*pair.second));
			_modelCache.insert(pair.first, model_ptr);
			model_vec->push_back(model_ptr);
		}
		_userModelCache.insert(userId, model_vec);
	}
}
