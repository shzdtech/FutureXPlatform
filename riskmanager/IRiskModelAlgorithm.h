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
	virtual int CheckRisk(ModelParamsDO& modelParams, IUserPositionContext& positionCtx, const std::string& userID) = 0;
};

typedef std::shared_ptr<IRiskModelAlgorithm> IRiskModelAlgorithm_Ptr;

#endif