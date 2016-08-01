/***********************************************************************
 * Module:  PricingAlgorithmManager.h
 * Author:  milk
 * Modified: 2015年8月9日 0:51:01
 * Purpose: Declaration of the class PricingAlgorithmManager
 ***********************************************************************/

#if !defined(__pricingengine_PricingAlgorithmManager_h)
#define __pricingengine_PricingAlgorithmManager_h

#include "IPricingAlgorithm.h"
#include "../utility/singleton_templ_mt.h"
#include <map>
#include "pricingengine_exp.h"

class PRICINGENGINE_CLASS_EXPORTS PricingAlgorithmManager : public singleton_mt_ptr<PricingAlgorithmManager>
{
public:
   IPricingAlgorithm_Ptr FindAlgorithm(const std::string& name) const;
   void InitializeInstance();

protected:

private:
   std::map<std::string, IPricingAlgorithm_Ptr> _algMap;


};

#endif