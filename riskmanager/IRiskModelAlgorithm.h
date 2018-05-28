#if !defined(__riskmanager_IRiskModelAlgorithm_h)
#define __riskmanager_IRiskModelAlgorithm_h

#include <string>
#include <memory>
#include "../pricingengine/IAlgorithmBase.h"
#include "../dataobject/ModelParamsDO.h"
#include "../common/typedefs.h"
#include "../ordermanager/IUserPositionContext.h"

class IRiskModelAlgorithm : public IAlgorithmBase
{
public:
	virtual int CheckRisk(const OrderRequestDO& orderReq, ModelParamsDO& modelParams, IUserPositionContext& positionCtx) = 0;
};

typedef std::shared_ptr<IRiskModelAlgorithm> IRiskModelAlgorithm_Ptr;

#endif