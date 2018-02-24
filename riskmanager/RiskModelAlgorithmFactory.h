/***********************************************************************
 * Module:  ModelAlgorithmFactory.h
 * Author:  milk
 * Modified: 2016年5月10日 14:59:09
 * Purpose: Declaration of the class ModelAlgorithmFactory
 ***********************************************************************/

#if !defined(__riskmanager_RiskModelAlgorithmFactory_h)
#define __riskmanager_RiskModelAlgorithmFactory_h

#include "IRiskModelAlgorithmFactory.h"

class RiskModelAlgorithmFactory : public IRiskModelAlgorithmFactory
{
public:
   std::vector<IRiskModelAlgorithm_Ptr> CreateModelAlgorithms(void);

protected:
private:

};

#endif