#if !defined(__dataobject_ModelInputDO_h)
#define __dataobject_ModelInputDO_h

#include "dataobjectbase.h"
#include "ContractKey.h"
#include <string>
#include <map>
#include <memory>

class ModelKey : public UserKey
{
public:
	ModelKey() = default;

	ModelKey(const std::string& instanceName, const std::string& userID)
		: InstanceName(instanceName), UserKey(userID) {}

	inline int compare(const ModelKey& modelKey) const
	{
		int cmp = InstanceName.compare(modelKey.InstanceName);
		return cmp != 0 ? cmp : _userID.compare(modelKey._userID);
	}

	bool operator< (const ModelKey& modelKey) const
	{
		return compare(modelKey) < 0;
	}

	bool operator== (const ModelKey& modelKey) const
	{
		return compare(modelKey) == 0;
	}

	std::string InstanceName;
};


class ModelParamsDO : public ModelKey, public dataobjectbase
{
public:
	ModelParamsDO() = default;

	ModelParamsDO(const std::string& instanceName, const std::string& modelType, const std::string& userID)
		: ModelKey(instanceName, userID), Model(modelType) {}

	std::string Model;

	std::map<std::string, double> Params;

	std::shared_ptr<void> ParsedParams;
};

typedef std::shared_ptr<ModelParamsDO> ModelParamsDO_Ptr;

#endif