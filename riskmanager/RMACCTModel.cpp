#include "RMACCTModel.h"
#include "../bizutility/ContractCache.h"



RMACCTModel::RMACCTModel()
{
}


RMACCTModel::~RMACCTModel()
{
}

const std::map<std::string, double>& RMACCTModel::DefaultParams(void) const
{
	static std::map<std::string, double> defaultParams = {
		{ RiskModelParams::__action__ , RiskModelParams::ActionType::NO_ACTION },
		{ RiskModelParams::__enabled__ , 1 },
		{ RiskModelParams::__interval__ , 0 },
		{ RiskModelParams::__match__ , 0 },
		{ RiskModelParams::__type__ , 0 },
		{ RiskModelParams::RiskRatio, 0.01 },
	};
	return defaultParams;
}


int RMACCTModel::CheckRisk(const OrderRequestDO& orderReq, ModelParamsDO& modelParams, IUserPositionContext* pPositionCtx, AccountInfoDO* pAccountInfo)
{
	if (auto pRiskModelParam = (RiskModelParams*)modelParams.ParsedParams.get())
	{
		if (pRiskModelParam->Enabled)
		{
			if (!pAccountInfo)
			{
				throw BizException(RiskModelParams::STOP_OPEN_ORDER, "Cannot find account information.");
			}

			auto riskRatio = modelParams.Params[RiskModelParams::RiskRatio];

			if (pAccountInfo->RiskRatio > riskRatio)
				throw BizException(RiskModelParams::STOP_OPEN_ORDER, "Risk ration exceeds.");
		}
	}

	return 0;
}

const std::string & RMACCTModel::Name(void) const
{
	static const std::string bs("rmacct");
	return bs;
}
