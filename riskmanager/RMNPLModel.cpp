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
	std::string exchange_underlying;
	int limitPos = 0;
	if (auto pInstrument = instrumentCache.QueryInstrumentOrAddById(orderReq.InstrumentID()))
	{
		exchange_underlying = pInstrument->ExchangeID() + '-' + pInstrument->ProductID;
		auto it = modelParams.Params.find(exchange_underlying);
		if (it == modelParams.Params.end())
		{
			throw BizException(RiskModelParams::STOP_OPEN_ORDER, exchange_underlying + "is not found in " + modelParams.InstanceName);
		}
		limitPos = it->second;
	}

	int real_position = 0;
	if (auto portfolioPositionPNL = pPositionCtx->GetPositionsByUser(orderReq.UserID(), orderReq.PortfolioID()))
	{
		for (auto pair : portfolioPositionPNL.map()->lock_table())
		{
			int position = pair.second->Position();
			if (auto pInstrument = instrumentCache.QueryInstrumentOrAddById(pair.second->InstrumentID()))
			{
				auto& underlying = pInstrument->ExchangeID() + '-' + pInstrument->ProductID;
				if (underlying == exchange_underlying)
					real_position += position;
			}
		}
	}

	if (!modelParams.ParsedParams)
		ParseParams(modelParams.Params, modelParams.ParsedParams);

	if (auto pRiskModelParam = (RiskModelParams*)modelParams.ParsedParams.get())
	{
		if (pRiskModelParam->Enabled)
		{
			std::string errMsg;
			auto totalPos = orderReq.Volume + real_position;
			if (totalPos > limitPos)
			{
				errMsg = modelParams.InstanceName + ": " + exchange_underlying + ": " + std::to_string(totalPos) + " > " + std::to_string(limitPos);
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
