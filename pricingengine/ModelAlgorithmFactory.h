/***********************************************************************
 * Module:  ModelAlgorithmFactory.h
 * Author:  milk
 * Modified: 2016年5月10日 14:59:09
 * Purpose: Declaration of the class ModelAlgorithmFactory
 ***********************************************************************/

#if !defined(__pricingengine_ModelAlgorithmFactory_h)
#define __pricingengine_ModelAlgorithmFactory_h

#include "IModelAlgorithmFactory.h"

class ModelAlgorithmFactory : public IModelAlgorithmFactory
{
public:
   std::vector<IModelAlgorithm_Ptr> CreateModelAlgorithms(void);

protected:
private:

};

#endif