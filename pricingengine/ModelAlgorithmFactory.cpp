/***********************************************************************
 * Module:  ModelAlgorithmFactory.cpp
 * Author:  milk
 * Modified: 2016年5月10日 14:59:09
 * Purpose: Implementation of the class ModelAlgorithmFactory
 ***********************************************************************/

#include "ModelAlgorithmFactory.h"
#include "BlackScholesIVM.h"
#include "BsBinIVM.h"
#include "WingsVolatilityModel.h"

////////////////////////////////////////////////////////////////////////
// Name:       ModelAlgorithmFactory::CreateAlgorithms()
// Purpose:    Implementation of ModelAlgorithmFactory::CreateAlgorithms()
// Return:     std::vector<IModelAlgorithm_Ptr>
////////////////////////////////////////////////////////////////////////

std::vector<IModelAlgorithm_Ptr> ModelAlgorithmFactory::CreateModelAlgorithms(void)
{
	std::vector<IModelAlgorithm_Ptr> ret;

	IModelAlgorithm_Ptr algptr;

	ret.push_back(std::make_shared<WingsVolatilityModel>());
	ret.push_back(std::make_shared<BlackScholesIVM>());
	ret.push_back(std::make_shared<BsBinIVM>());
	
	return ret;
}