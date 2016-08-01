#include "OTCOptionVolatility.h"
#include "../pricingengine/ModelAlgorithmManager.h"

dataobj_ptr OTCOptionVolatility::HandleRequest(const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	CheckLogin(session);
	dataobj_ptr ret;

	if(auto pModelDO = (ModelParamsDO*)reqDO.get())
	{ 
		if (auto volatilityModelPtr =
			ModelAlgorithmManager::Instance()->FindModelAlgorithm(pModelDO->ModelName))
		{
			ret = volatilityModelPtr->Compute(reqDO);
		}
		else
		{
			throw NotFoundException("Model: " + pModelDO->ModelName + "not found!");
		}
	}

	return ret;
}
