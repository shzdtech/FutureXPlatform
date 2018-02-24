#pragma once
#include "../dataobject/ModelParamDefDO.h"
#include "../dataobject/TypedefDO.h"
#include "../common/typedefs.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT ModelParamDefCache
{
public:
	static ModelParamDefDO_Ptr FindModelParamDef(const std::string& key);

	static ModelParamDefDO_Ptr FindOrRetrieveModelParamDef(const std::string& key);

	static void Remove(const std::string& key);

	static void Clear(void);

	static void Load(void);
};

