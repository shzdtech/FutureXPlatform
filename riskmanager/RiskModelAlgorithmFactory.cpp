/***********************************************************************
 * Module:  ModelAlgorithmFactory.cpp
 * Author:  milk
 * Modified: 2016年5月10日 14:59:09
 * Purpose: Implementation of the class ModelAlgorithmFactory
 ***********************************************************************/

#include "RiskModelAlgorithmFactory.h"
#include "RMNPLModel.h"


////////////////////////////////////////////////////////////////////////
// Name:       ModelAlgorithmFactory::CreateAlgorithms()
// Purpose:    Implementation of ModelAlgorithmFactory::CreateAlgorithms()
// Return:     std::vector<IModelAlgorithm_Ptr>
////////////////////////////////////////////////////////////////////////

std::vector<IRiskModelAlgorithm_Ptr> RiskModelAlgorithmFactory::CreateModelAlgorithms(void)
{
	std::vector<IRiskModelAlgorithm_Ptr> ret;

	ret.push_back(std::make_shared<RMNPLModel>());
	
	return ret;
}