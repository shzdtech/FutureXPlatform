#if !defined(__databaseop_ModelParamsDAO_h)
#define __databaseop_ModelParamsDAO_h

#include <string>
#include "../dataobject/ModelParamsDO.h"
#include "../dataobject/ModelParamDefDO.h"
#include "../dataobject/TemplateDO.h"
#include "../utility/autofillmap.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS ModelParamsDAO
{
public:
	static ModelParamsDO_Ptr FindUserModel(const std::string& userid, const std::string& modelInstance);
	static void FindAllModels(const std::string& userId, autofillmap<ModelKey, ModelParamsDO_Ptr>& modelMap);
	static void SaveModelParams(const ModelParamsDO& modelParams);
	static void NewUserModel(const ModelParamsDO & modelParams);
	static ModelParamDefDO_Ptr FindModelParamDef(const std::string& modelName);
};

#endif