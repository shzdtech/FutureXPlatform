#pragma once
#include "RiskModelBase.h"
class RMNPLModel : public RiskModelBase
{
public:
	RMNPLModel();
	~RMNPLModel();

	virtual int CheckRisk(const OrderRequestDO& orderReq, ModelParamsDO& modelParams, IUserPositionContext* pPositionCtx, AccountInfoDO* pAccountInfo);
	virtual const std::string& Name(void) const;

public:

};

