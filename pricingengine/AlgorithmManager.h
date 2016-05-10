/***********************************************************************
 * Module:  AlgorithmManager.h
 * Author:  milk
 * Modified: 2015年8月9日 0:51:01
 * Purpose: Declaration of the class AlgorithmManager
 ***********************************************************************/

#if !defined(__strategy_AlgorithmManager_h)
#define __strategy_AlgorithmManager_h

#include "IAlgorithm.h"
#include "../utility/singleton_templ_mt.h"
#include <map>
#include "pricingengine_exp.h"

class PRICINGENGINE_CLASS_EXPORTS AlgorithmManager : public singleton_mt_ptr<AlgorithmManager>
{
public:
   IAlgorithm_Ptr FindAlgorithm(const std::string& name) const;
   void Initialize();

protected:

private:
   std::map<std::string, IAlgorithm_Ptr> _algMap;


};

#endif