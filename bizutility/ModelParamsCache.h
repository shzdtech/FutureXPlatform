#pragma once
#include "../dataobject/ModelParamsDO.h"
#include "../dataobject/TypedefDO.h"
#include "../common/typedefs.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT ModelParamsCache
{
public:
	static ModelParamsDO_Ptr FindModel(const ModelKey& key);

	static ModelParamsDO_Ptr FindTempModel(const ModelKey & key);

	static bool InsertTempModel(const ModelParamsDO_Ptr& model);

	static ModelParamsDO_Ptr FindOrRetrieveModel(const ModelKey & key);

	static vector_ptr<ModelParamsDO_Ptr> FindModelsByUser(const std::string& userId);

	static void Remove(const ModelKey& key);

	static bool RemoveTempModel(const ModelKey& key);

	static void Clear(void);
	static void Load(const std::string & userId);
};

