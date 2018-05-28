#include "RMNPLModel.h"
#include "../bizutility/ContractCache.h"



RMNPLModel::RMNPLModel()
{
}


RMNPLModel::~RMNPLModel()
{
}


int RMNPLModel::CheckRisk(const OrderRequestDO& orderReq, ModelParamsDO& modelParams, IUserPositionContext& positionCtx)
{
	auto& instrumentCache = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE);
	autofillmap<std::string, int> positionMap;
	if (auto portfolioPositionPNL = positionCtx.GetPortfolioPositionsPnLByUser(orderReq.UserID()))
	{	
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

	if (auto pInstrument = instrumentCache.QueryInstrumentOrAddById(orderReq.InstrumentID()))
	{
		positionMap.getorfill(orderReq.ExchangeID() + '-' + pInstrument->ProductID) += orderReq.Volume;
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
							switch (pRiskModelParam->Action)
							{
							case RiskModelParams::STOP_OPEN_ORDER:
								throw BizException(RiskModelParams::STOP_OPEN_ORDER, modelParams.InstanceName + ": " + pair.first + ": " + std::to_string(position) + ">" + std::to_string(limitPos));
							case RiskModelParams::WARNING:
								throw BizException(RiskModelParams::WARNING, "Warining: " + modelParams.InstanceName + ": " + pair.first + ": " + std::to_string(position) + ">" + std::to_string(limitPos));
							default:
								return pRiskModelParam->Action;
							}
						}
					}
					else
					{
						matchAll = false;
					}
				}
				else
				{
					matchAll = false;
				}
			}
			if (matchAll)
			{
				switch (pRiskModelParam->Action)
				{
				case RiskModelParams::STOP_OPEN_ORDER:
					throw BizException(RiskModelParams::STOP_OPEN_ORDER, modelParams.InstanceName + " 'match all' is violated!");
				case RiskModelParams::WARNING:
					throw BizException(RiskModelParams::WARNING, "Warning: " + modelParams.InstanceName + " 'match all' is violated!");
				default:
					return pRiskModelParam->Action;
				}
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
