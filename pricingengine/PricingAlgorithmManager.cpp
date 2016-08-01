/***********************************************************************
 * Module:  PricingAlgorithmManager.cpp
 * Author:  milk
 * Modified: 2015年8月9日 0:51:01
 * Purpose: Implementation of the class PricingAlgorithmManager
 ***********************************************************************/

#include "PricingAlgorithmManager.h"
#include "PricingAlgorithmFactory.h"

std::once_flag PricingAlgorithmManager::_instance_flag;
std::shared_ptr<PricingAlgorithmManager> PricingAlgorithmManager::_instance = nullptr;

////////////////////////////////////////////////////////////////////////
// Name:       PricingAlgorithmManager::FindAlgorithm(const std::string& name)
// Purpose:    Implementation of PricingAlgorithmManager::FindAlgorithm()
// Parameters:
// - name
// Return:     IPricingAlgorithm_Ptr
////////////////////////////////////////////////////////////////////////

IPricingAlgorithm_Ptr PricingAlgorithmManager::FindAlgorithm(const std::string& name) const
{
	IPricingAlgorithm_Ptr ret;
	auto it = _algMap.find(name);
	if (it != _algMap.end())
	{
		ret = it->second;
	}
	return ret;
}

void PricingAlgorithmManager::InitializeInstance()
{
	auto algVect = PricingAlgorithmFactory().CreateAlgorithms();

	for (auto& it : algVect)
	{
		_algMap.emplace(it->Name(), it);
	}
}