/***********************************************************************
 * Module:  IModelAlgorithmFactory.h
 * Author:  milk
 * Modified: 2016年5月9日 23:16:36
 * Purpose: Declaration of the class IModelAlgorithmFactory
 ***********************************************************************/

#if !defined(__riskmanager_IRiskModelAlgorithmFactory_h)
#define __riskmanager_IRiskModelAlgorithmFactory_h

#include <vector>
#include "IRiskModelAlgorithm.h"

class IRiskModelAlgorithmFactory
{
public:
   virtual std::vector<IRiskModelAlgorithm_Ptr> CreateModelAlgorithms(void) = 0;

protected:
private:

};

#endif