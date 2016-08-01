/***********************************************************************
 * Module:  ModelAlgorithmManager.h
 * Author:  milk
 * Modified: 2015年8月9日 0:51:01
 * Purpose: Declaration of the class ModelAlgorithmManager
 ***********************************************************************/

#if !defined(__pricingengine_ModelAlgorithmManager_h)
#define __pricingengine_ModelAlgorithmManager_h

#include "IModelAlgorithm.h"
#include "../utility/singleton_templ_mt.h"
#include <map>
#include "pricingengine_exp.h"

class PRICINGENGINE_CLASS_EXPORTS ModelAlgorithmManager : public singleton_mt_ptr<ModelAlgorithmManager>
{
public:
	IModelAlgorithm_Ptr FindModelAlgorithm(const std::string& name) const;
	void InitializeInstance();

protected:

private:
	std::map<std::string, IModelAlgorithm_Ptr> _algMap;


};

#endif