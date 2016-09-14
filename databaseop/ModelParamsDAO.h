#pragma once
#include <string>
#include "../dataobject/ModelParamsDO.h"
#include "../dataobject/TemplateDO.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS ModelParamsDAO
{
public:
	static ModelParamsDO_Ptr FindUserModel(const std::string& userid, const std::string& modelInstance);
	static void SaveModelParams(const ModelParamsDO& modelParams);
};

