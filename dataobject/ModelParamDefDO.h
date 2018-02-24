#if !defined(__dataobject_ModelParamDefDO_h)
#define __dataobject_ModelParamDefDO_h

#include "dataobjectbase.h"
#include "ParamsBase.h"
#include <string>
#include <map>
#include <memory>

struct ModelParamDef
{
	double DefaultVal;
	double MinVal = NAN;
	double MaxVal = NAN;
	std::string StringVal;
	double Step = 0;
	int Digits = 0;
	int DataType = 2;
	bool Visible = true;
	bool Enable = true;
};


class ModelParamDefDO : public dataobjectbase
{
public:
	ModelParamDefDO() = default;

	ModelParamDefDO(const std::string& modelName) 
		: ModelName(modelName) {};

public:
	std::map<std::string, ModelParamDef> ModelDefMap;

	std::string ModelName;
};


typedef std::shared_ptr<ModelParamDefDO> ModelParamDefDO_Ptr;

#endif