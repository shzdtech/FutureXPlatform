/***********************************************************************
 * Module:  AlgorithmManager.cpp
 * Author:  milk
 * Modified: 2015年8月9日 0:51:01
 * Purpose: Implementation of the class AlgorithmManager
 ***********************************************************************/

#include "AlgorithmManager.h"
#include "BetaSumAlgorithm.h"
#include "ETPAlgorithm.h"

std::once_flag AlgorithmManager::_instance_flag;
std::shared_ptr<AlgorithmManager> AlgorithmManager::_instance = nullptr;

////////////////////////////////////////////////////////////////////////
// Name:       AlgorithmManager::FindAlgorithm(const std::string& name)
// Purpose:    Implementation of AlgorithmManager::FindAlgorithm()
// Parameters:
// - name
// Return:     IAlgorithm_Ptr
////////////////////////////////////////////////////////////////////////

IAlgorithm_Ptr AlgorithmManager::FindAlgorithm(const std::string& name)
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
	IAlgorithm_Ptr algptr;
	
	algptr = std::make_shared<BetaSumAlgorithm>();
	_algMap[algptr->Name()] = algptr;

	algptr = std::make_shared<ETPAlgorithm>();
	_algMap[algptr->Name()] = algptr;
}