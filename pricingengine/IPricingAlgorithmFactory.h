/***********************************************************************
 * Module:  IPricingAlgorithmFactory.h
 * Author:  milk
 * Modified: 2016年5月9日 23:16:36
 * Purpose: Declaration of the class IPricingAlgorithmFactory
 ***********************************************************************/

#if !defined(__pricingengine_IPricingAlgorithmFactory_h)
#define __pricingengine_IPricingAlgorithmFactory_h

#include <vector>
#include "IPricingAlgorithm.h"

class IPricingAlgorithmFactory
{
public:
   virtual std::vector<IPricingAlgorithm_Ptr> CreateAlgorithms(void)=0;

protected:
private:

};

#endif