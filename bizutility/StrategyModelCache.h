#pragma once
#include "../dataobject/ModelParamsDO.h"
#include "../dataobject/TypedefDO.h"
#include "../common/typedefs.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT StrategyModelCache
{
public:
	static ModelParamsDO_Ptr FindModel(const ModelKey& key);

	static ModelParamsDO_Ptr FindTempModel(const ModelKey & key);

	static bool InsertTempModel(const ModelParamsDO_Ptr& model);

	static ModelParamsDO_Ptr FindOrRetrieveModel(const ModelKey & key);

	static const std::map<ModelKey, ModelParamsDO_Ptr>& ModelCache();

	static vector_ptr<ModelParamsDO_Ptr> FindModelsByUser(const std::string& userId);

	static void Remove(const ModelKey& key);

	static bool RemoveTempModel(const ModelKey& key);

	static void Clear(void);

private:
	class static_initializer
	{
	public:
		static_initializer();
	};

	static static_initializer _static_init;
};

