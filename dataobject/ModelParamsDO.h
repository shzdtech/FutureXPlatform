#if !defined(__dataobject_ModelInputDO_h)
#define __dataobject_ModelInputDO_h

#include "dataobjectbase.h"

#include <string>
#include <vector>

class ModelParamsDO : public dataobjectbase
{
public:
	std::string ModelName;

	std::map<std::string, double> ScalaParams;
	std::map<std::string, std::vector<double>> VectorParams;
};

#endif