/***********************************************************************
 * Module:  ModelAlgorithmManager.cpp
 * Author:  milk
 * Modified: 2015年8月9日 0:51:01
 * Purpose: Implementation of the class ModelAlgorithmManager
 ***********************************************************************/

#include "RiskModelAlgorithmManager.h"
#include "RiskModelAlgorithmFactory.h"

//std::once_flag ModelAlgorithmManager::_instance_flag;
//std::shared_ptr<ModelAlgorithmManager> ModelAlgorithmManager::_instance = nullptr;

////////////////////////////////////////////////////////////////////////
// Name:       ModelAlgorithmManager::FindModel(const std::string& name)
// Purpose:    Implementation of ModelAlgorithmManager::FindModel()
// Parameters:
// - name
// Return:     IPricingAlgorithm_Ptr
////////////////////////////////////////////////////////////////////////

IRiskModelAlgorithm_Ptr RiskModelAlgorithmManager::FindModel(const std::string& name) const
{
	IRiskModelAlgorithm_Ptr ret;
	auto it = _algMap.find(name);
	if (it != _algMap.end())
	{
		ret = it->second;
	}
	return ret;
}

void RiskModelAlgorithmManager::InitializeInstance()
{
	auto algVect = RiskModelAlgorithmFactory().CreateModelAlgorithms();

	for (auto& it : algVect)
	{
		_algMap.emplace(it->Name(), it);
	}
}