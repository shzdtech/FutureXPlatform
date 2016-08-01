/***********************************************************************
 * Module:  IModelAlgorithmFactory.h
 * Author:  milk
 * Modified: 2016年5月9日 23:16:36
 * Purpose: Declaration of the class IModelAlgorithmFactory
 ***********************************************************************/

#if !defined(__pricingengine_IModelAlgorithmFactory_h)
#define __pricingengine_IModelAlgorithmFactory_h

#include <vector>
#include "IModelAlgorithm.h"

class IModelAlgorithmFactory
{
public:
   virtual std::vector<IModelAlgorithm_Ptr> CreateModelAlgorithms(void) = 0;

protected:
private:

};

#endif