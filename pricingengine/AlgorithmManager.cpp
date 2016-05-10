/***********************************************************************
 * Module:  AlgorithmManager.cpp
 * Author:  milk
 * Modified: 2015年8月9日 0:51:01
 * Purpose: Implementation of the class AlgorithmManager
 ***********************************************************************/

#include "AlgorithmManager.h"
#include "PricingAlgorithmFactory.h"

std::once_flag AlgorithmManager::_instance_flag;
std::shared_ptr<AlgorithmManager> AlgorithmManager::_instance = nullptr;

////////////////////////////////////////////////////////////////////////
// Name:       AlgorithmManager::FindAlgorithm(const std::string& name)
// Purpose:    Implementation of AlgorithmManager::FindAlgorithm()
// Parameters:
// - name
// Return:     IAlgorithm_Ptr
////////////////////////////////////////////////////////////////////////

IAlgorithm_Ptr AlgorithmManager::FindAlgorithm(const std::string& name) const
{
	IAlgorithm_Ptr ret;
	auto it = _algMap.find(name);
	if (it != _algMap.end())
	{
		ret = it->second;
	}
	return ret;
}

void AlgorithmManager::Initialize()
{
	auto algVect = PricingAlgorithmFactory().CreateAlgorithms();

	for (auto& it : algVect)
	{
		_algMap.emplace(it->Name(), it);
	}
}