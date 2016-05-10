/***********************************************************************
 * Module:  PricingAlgorithmFactory.h
 * Author:  milk
 * Modified: 2016年5月10日 14:59:09
 * Purpose: Declaration of the class PricingAlgorithmFactory
 ***********************************************************************/

#if !defined(__pricingengine_PricingAlgorithmFactory_h)
#define __pricingengine_PricingAlgorithmFactory_h

#include "IAlgorithmFactory.h"

class PricingAlgorithmFactory : public IAlgorithmFactory
{
public:
   std::vector<IAlgorithm_Ptr> CreateAlgorithms(void);

protected:
private:

};

#endif