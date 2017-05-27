/***********************************************************************
 * Module:  StrategyConstractDAO.h
 * Author:  milk
 * Modified: 2015年8月8日 22:30:32
 * Purpose: Declaration of the class StrategyConstractDAO
 ***********************************************************************/

#if !defined(__databaseop_StrategyContractDAO_h)
#define __databaseop_StrategyContractDAO_h

#include "../dataobject/ContractKey.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/ContractParamDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/TypedefDO.h"
#include "../utility/autofillmap.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS StrategyContractDAO
{
public:
	static VectorDO_Ptr<StrategyContractDO_Ptr> LoadStrategyContractByProductType(int productType);
	static void RetrievePricingContractsByProductType(int productType, const std::string& modelaim, 
		autofillmap<UserStrategyName, autofillmap<ContractKey, StrategyPricingContract_Ptr>>& pricingContractMap);
	static void RetrieveStrategyModels(autofillmap<UserStrategyName, autofillmap<std::string, std::string>>& strategyDOMap);
	static bool FindStrategyModelByAim(const std::string& strategySym, const std::string& aim, const std::string& userid, std::string& modelinstance);
	static void UpdateStrategy(const StrategyContractDO& strategyDO);
	static void UpdateStrategyModel(const StrategyContractDO & strategyDO);
	static void UpdatePricingContract(const UserContractKey& userContractKey, const std::string& strategySymb,
		const std::string& modelAim, const StrategyPricingContract& sto);
	static VectorDO_Ptr<ContractParamDO> RetrieveContractParamByUser(const std::string & userid, int productType);

protected:
private:

};

#endif