#include "RMNPLModel.h"
#include "../bizutility/ContractCache.h"



RMNPLModel::RMNPLModel()
{
}


RMNPLModel::~RMNPLModel()
{
}


int RMNPLModel::CheckRisk(ModelParamsDO & modelParams, IUserPositionContext & positionCtx, const std::string & userID)
{
	autofillmap<std::string, int> positionMap;
	if(auto portfolioPositionPNL = positionCtx.GetPortfolioPositionsPnLByUser(userID))
	{
		auto& instrumentCache = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE);
		for (auto pair : portfolioPositionPNL.map()->lock_table())
		{
			for (auto pPair : pair.second.map()->lock_table())
			{
				int position = pPair.second->NetPosition();
				if (auto pInstrument = instrumentCache.QueryInstrumentOrAddById(pPair.second->InstrumentID()))
				{
					positionMap.getorfill(pPair.second->ExchangeID() + '-' + pInstrument->ProductID) += position;
				}
			}
		}
	}

	if (!modelParams.ParsedParams)
		ParseParams(modelParams.Params, modelParams.ParsedParams);

	if (auto pRiskModelParam = (RiskModelParams*)modelParams.ParsedParams.get())
	{
		if (pRiskModelParam->Enabled)
		{
			bool matchAll = true;
			for (auto pair : modelParams.Params)
			{
				if (auto pPos = positionMap.tryfind(pair.first))
				{
					auto position = std::labs(*pPos);
					auto limitPos = (long)pair.second;
					if (position > limitPos)
					{
						if (pRiskModelParam->MatchAny)
						{
							throw BizException(modelParams.InstanceName + ": " + pair.first + ": " + std::to_string(position) + ">" + std::to_string(limitPos));
						}
					}
					else
					{
						matchAll = false;
					}
				}
			}
			if (matchAll)
			{
				throw BizException(modelParams.InstanceName + " is violated!");
			}
		}
	}

	return 0;
}

const std::string & RMNPLModel::Name(void) const
{
	static const std::string bs("rmnpl");
	return bs;
}
