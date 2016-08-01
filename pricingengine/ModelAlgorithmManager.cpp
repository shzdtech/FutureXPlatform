/***********************************************************************
 * Module:  ModelAlgorithmManager.cpp
 * Author:  milk
 * Modified: 2015年8月9日 0:51:01
 * Purpose: Implementation of the class ModelAlgorithmManager
 ***********************************************************************/

#include "ModelAlgorithmManager.h"
#include "ModelAlgorithmFactory.h"

std::once_flag ModelAlgorithmManager::_instance_flag;
std::shared_ptr<ModelAlgorithmManager> ModelAlgorithmManager::_instance = nullptr;

////////////////////////////////////////////////////////////////////////
// Name:       ModelAlgorithmManager::FindAlgorithm(const std::string& name)
// Purpose:    Implementation of ModelAlgorithmManager::FindAlgorithm()
// Parameters:
// - name
// Return:     IPricingAlgorithm_Ptr
////////////////////////////////////////////////////////////////////////

IModelAlgorithm_Ptr ModelAlgorithmManager::FindModelAlgorithm(const std::string& name) const
{
	IModelAlgorithm_Ptr ret;
	auto it = _algMap.find(name);
	if (it != _algMap.end())
	{
		ret = it->second;
	}
	return ret;
}

void ModelAlgorithmManager::InitializeInstance()
{
	auto algVect = ModelAlgorithmFactory().CreateModelAlgorithms();

	for (auto& it : algVect)
	{
		_algMap.emplace(it->Name(), it);
	}
}