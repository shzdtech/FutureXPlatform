/***********************************************************************
 * Module:  PricingAlgorithmFactory.cpp
 * Author:  milk
 * Modified: 2016年5月10日 14:59:09
 * Purpose: Implementation of the class PricingAlgorithmFactory
 ***********************************************************************/

#include "PricingAlgorithmFactory.h"

#include "BetaSumPricingAlgorithm.h"
#include "ETPPricingAlgorithm.h"
#include "BlackScholesPricingAlgorithm.h"
#include "BsBinPricingAlgorithm.h"

////////////////////////////////////////////////////////////////////////
// Name:       PricingAlgorithmFactory::CreateAlgorithms()
// Purpose:    Implementation of PricingAlgorithmFactory::CreateAlgorithms()
// Return:     std::vector<IPricingAlgorithm_Ptr>
////////////////////////////////////////////////////////////////////////

std::vector<IPricingAlgorithm_Ptr> PricingAlgorithmFactory::CreateAlgorithms(void)
{
	std::vector<IPricingAlgorithm_Ptr> ret;

	ret.push_back(std::make_shared<BetaSumPricingAlgorithm>());
	ret.push_back(std::make_shared<ETPPricingAlgorithm>());
	ret.push_back(std::make_shared<BlackScholesPricingAlgorithm>());
	ret.push_back(std::make_shared<BsBinPricingAlgorithm>());
	
	return ret;
}