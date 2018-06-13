#pragma once

#include "../dataobject/OrderDO.h"
#include "../dataobject/AccountInfoDO.h"
#include "../dataobject/MessageExceptionDO.h"
#include "RiskModelParams.h"
#include "RiskContext.h"
#include "RiskModelAlgorithmManager.h"

#include "riskmanager_exp.h"

class RISKMANAGER_CLASS_EXPORTS RiskUtil
{
public:
	static int CheckRisk(const OrderRequestDO & orderReq, IUserPositionContext* pUserPosition, AccountInfoDO* pAccount);
};

