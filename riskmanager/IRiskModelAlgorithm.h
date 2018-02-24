#if !defined(__riskmanager_IRiskModelAlgorithm_h)
#define __riskmanager_IRiskModelAlgorithm_h

#include <string>
#include <memory>
#include "IRiskModelBase.h"
#include "../dataobject/ModelParamsDO.h"
#include "../common/typedefs.h"
#include "../ordermanager/IUserPositionContext.h"

class IRiskModelAlgorithm : public IRiskModelBase
{
public:
	virtual bool CheckRisk(const ModelParamsDO& modelParams, const IUserPositionContext& positionCtx, const std::string& userID) = 0;
};

typedef std::shared_ptr<IRiskModelAlgorithm> IRiskModelAlgorithm_Ptr;

#endif