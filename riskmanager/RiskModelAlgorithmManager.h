/***********************************************************************
 * Module:  ModelAlgorithmManager.h
 * Author:  milk
 * Modified: 2015年8月9日 0:51:01
 * Purpose: Declaration of the class ModelAlgorithmManager
 ***********************************************************************/

#if !defined(__riskmanager_RiskModelAlgorithmManager_h)
#define __riskmanager_RiskModelAlgorithmManager_h

#include "IRiskModelAlgorithm.h"
#include "../utility/singleton_templ_mt.h"
#include <map>
#include "riskmanager_exp.h"

class RISKMANAGER_CLASS_EXPORTS RiskModelAlgorithmManager : public singleton_mt_ptr<RiskModelAlgorithmManager>
{
public:
	IRiskModelAlgorithm_Ptr FindModel(const std::string& name) const;
	void InitializeInstance();

protected:

private:
	std::map<std::string, IRiskModelAlgorithm_Ptr> _algMap;


};

#endif