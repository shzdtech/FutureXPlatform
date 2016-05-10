/***********************************************************************
 * Module:  PricingAlgorithmFactory.cpp
 * Author:  milk
 * Modified: 2016年5月10日 14:59:09
 * Purpose: Implementation of the class PricingAlgorithmFactory
 ***********************************************************************/

#include "PricingAlgorithmFactory.h"

#include "BetaSumAlgorithm.h"
#include "ETPAlgorithm.h"
#include "BlackScholesPricing.h"

////////////////////////////////////////////////////////////////////////
// Name:       PricingAlgorithmFactory::CreateAlgorithms()
// Purpose:    Implementation of PricingAlgorithmFactory::CreateAlgorithms()
// Return:     std::vector<IAlgorithm_Ptr>
////////////////////////////////////////////////////////////////////////

std::vector<IAlgorithm_Ptr> PricingAlgorithmFactory::CreateAlgorithms(void)
{
	std::vector<IAlgorithm_Ptr> ret;

	IAlgorithm_Ptr algptr;

	ret.push_back(std::make_shared<BetaSumAlgorithm>());
	ret.push_back(std::make_shared<ETPAlgorithm>());
	ret.push_back(std::make_shared<BlackScholesPricing>());
	
	return ret;
}