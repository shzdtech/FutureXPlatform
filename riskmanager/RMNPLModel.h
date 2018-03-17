#pragma once
#include "RiskModelBase.h"
class RMNPLModel : public RiskModelBase
{
public:
	RMNPLModel();
	~RMNPLModel();

	virtual int CheckRisk(ModelParamsDO& modelParams, IUserPositionContext& positionCtx, const std::string& userID);
	virtual const std::string& Name(void) const;

public:

};

