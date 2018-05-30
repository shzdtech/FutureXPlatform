#include "RMNPLModel.h"
#include "../bizutility/ContractCache.h"



RMNPLModel::RMNPLModel()
{
}


RMNPLModel::~RMNPLModel()
{
}


int RMNPLModel::CheckRisk(const OrderRequestDO& orderReq, ModelParamsDO& modelParams, IUserPositionContext* pPositionCtx, AccountInfoDO* pAccountInfo)
{
	auto& instrumentCache = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE);
	if (auto pInstrument = instrumentCache.QueryInstrumentOrAddById(orderReq.InstrumentID()))
	{
		auto& underlying = pInstrument->ExchangeID() + '-' + pInstrument->ProductID;
		if (modelParams.Params.find(underlying) == modelParams.Params.end())
		{
			throw BizException(RiskModelParams::STOP_OPEN_ORDER, underlying + "is not found in " + modelParams.InstanceName);
		}
	}

	autofillmap<std::string, int> positionMap;
	if (auto portfolioPositionPNL = pPositionCtx->GetPositionsByUser(orderReq.UserID(), orderReq.PortfolioID()))
	{
		for (auto pair : portfolioPositionPNL.map()->lock_table())
		{
			int position = pair.second->Position();
			if (auto pInstrument = instrumentCache.QueryInstrumentOrAddById(pair.second->InstrumentID()))
			{
				auto& underlying = pInstrument->ExchangeID() + '-' + pInstrument->ProductID;
				if (modelParams.Params.find(underlying) != modelParams.Params.end())
					positionMap.getorfill(underlying) += position;
			}
		}
	}

	if (!modelParams.ParsedParams)
		ParseParams(modelParams.Params, modelParams.ParsedParams);

	if (auto pRiskModelParam = (RiskModelParams*)modelParams.ParsedParams.get())
	{
		if (pRiskModelParam->Enabled)
		{
			bool throwErr = false;
			std::string errMsg;
			for (auto pair : modelParams.Params)
			{
				if (auto pPos = positionMap.tryfind(pair.first))
				{
					auto position = std::abs(*pPos);
					auto limitPos = (int)pair.second;
					auto totalPos = orderReq.Volume + position;
					if (totalPos > limitPos)
					{
						if (pRiskModelParam->MatchAny)
						{
							errMsg = modelParams.InstanceName + ": " + pair.first + ": " + std::to_string(totalPos) + " > " + std::to_string(limitPos);
							throwErr = true;
							break;
						}
					}
				}
			}
			if (throwErr)
			{
				switch (pRiskModelParam->Action)
				{
				case RiskModelParams::STOP_OPEN_ORDER:
					throw BizException(RiskModelParams::STOP_OPEN_ORDER, errMsg);
				case RiskModelParams::WARNING:
					throw BizException(RiskModelParams::WARNING, "Warning: " + errMsg);
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
