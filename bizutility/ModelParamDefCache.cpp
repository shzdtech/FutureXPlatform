#include "ModelParamDefCache.h"
#include "../utility/autofillmap.h"
#include "../include/libcuckoo/cuckoohash_map.hh"
#include "../databaseop/ModelParamsDAO.h"

static cuckoohash_map<std::string, ModelParamDefDO_Ptr> _modelParamDefCache(128);

ModelParamDefDO_Ptr ModelParamDefCache::FindModelParamDef(const std::string & key)
{
	ModelParamDefDO_Ptr ret;
	_modelParamDefCache.find(key, ret);
	return ret;
}

ModelParamDefDO_Ptr ModelParamDefCache::FindOrRetrieveModelParamDef(const std::string & key)
{
	auto ret = FindModelParamDef(key);
	if (!ret)
	{
		if (ret = ModelParamsDAO::FindModelParamDef(key))
		{
			_modelParamDefCache.insert_or_assign(ret->ModelName, ret);
		}
	}

	return ret;
}

void ModelParamDefCache::Remove(const std::string & key)
{
	_modelParamDefCache.erase(key);
}

void ModelParamDefCache::Clear(void)
{
	_modelParamDefCache.clear();
}

void ModelParamDefCache::Load(void)
{
}
