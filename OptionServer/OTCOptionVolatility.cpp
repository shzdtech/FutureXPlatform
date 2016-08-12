#include "OTCOptionVolatility.h"
#include "../dataobject/StrategyContractDO.h"
#include "../pricingengine/ModelAlgorithmManager.h"

dataobj_ptr OTCOptionVolatility::HandleRequest(const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	CheckLogin(session);
	dataobj_ptr ret;

	if(auto pStrategy = (StrategyContractDO*)reqDO.get())
	{ 
		if (auto volatilityModelPtr =
			ModelAlgorithmManager::Instance()->FindModelAlgorithm(pStrategy->ModelParams.ModelName))
		{
			std::vector<double> moneyVector;
			ret = volatilityModelPtr->Compute(&moneyVector, pStrategy->ModelParams, nullptr);
		}
		else
		{
			throw NotFoundException("Model: " + pStrategy->ModelParams.ModelName + "not found!");
		}
	}

	return ret;
}
