/***********************************************************************
 * Module:  ModelAlgorithmFactory.cpp
 * Author:  milk
 * Modified: 2016年5月10日 14:59:09
 * Purpose: Implementation of the class ModelAlgorithmFactory
 ***********************************************************************/

#include "RiskModelAlgorithmFactory.h"


////////////////////////////////////////////////////////////////////////
// Name:       ModelAlgorithmFactory::CreateAlgorithms()
// Purpose:    Implementation of ModelAlgorithmFactory::CreateAlgorithms()
// Return:     std::vector<IModelAlgorithm_Ptr>
////////////////////////////////////////////////////////////////////////

std::vector<IRiskModelAlgorithm_Ptr> RiskModelAlgorithmFactory::CreateModelAlgorithms(void)
{
	std::vector<IRiskModelAlgorithm_Ptr> ret;

	IRiskModelAlgorithm_Ptr algptr;
	
	return ret;
}