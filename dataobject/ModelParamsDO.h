#if !defined(__dataobject_ModelInputDO_h)
#define __dataobject_ModelInputDO_h

#include "dataobjectbase.h"
#include "ContractKey.h"
#include "ParamsBase.h"
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

	ModelParamsDO(const ModelParamsDO& modelParams)
	{
		this->InstanceName = modelParams.InstanceName;
		this->_userID = modelParams._userID;
		this->Model = modelParams.Model;
		this->ModelAim = modelParams.ModelAim;
		this->Params = modelParams.Params;
		this->ParamString = modelParams.ParamString;
	}

	ModelParamsDO(const std::string& instanceName, const std::string& modelType, const std::string& userID)
		: ModelKey(instanceName, userID), Model(modelType) {}

	std::string Model;

	std::string ModelAim;

	std::map<std::string, double> Params;

	std::map<std::string, std::string> ParamString;

	std::unique_ptr<ParamsBase> ParsedParams;
};

class ModelKeyHash
{
public:
	std::size_t operator()(const ModelKey& k) const {
		static std::hash<std::string> hasher;
		return hasher(k.InstanceName) ^ hasher(k.UserID());
	}
};

typedef std::shared_ptr<ModelParamsDO> ModelParamsDO_Ptr;

#endif