#pragma once
#include "RiskModelBase.h"
class RMACCTModel : public RiskModelBase
{
public:
	RMACCTModel();
	~RMACCTModel();

	virtual const std::map<std::string, double>& DefaultParams(void) const;

	virtual int CheckRisk(const OrderRequestDO& orderReq, ModelParamsDO& modelParams, IUserPositionContext* pPositionCtx, AccountInfoDO* pAccountInfo);
	virtual const std::string& Name(void) const;

public:

};

