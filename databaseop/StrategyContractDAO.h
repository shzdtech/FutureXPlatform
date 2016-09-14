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
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS StrategyContractDAO
{
public:
	static VectorDO_Ptr<StrategyContractDO> FindStrategyContractByUser(const std::string& userid, int productType);
	static void RetrievePricingContracts(const std::string& strategyExchange, const std::string& strategyContract, const std::string& userid, std::vector<PricingContract>& pricingContracts);
	static void RetrieveStrategyModels(const std::string& strategySym, const std::string& userid, StrategyContractDO& sto);
	static bool FindStrategyModelByAim(const std::string& strategySym, const std::string& aim, const std::string& userid, std::string& modelinstance);
	static VectorDO_Ptr<ContractParamDO> RetrieveContractParamByUser(const std::string & userid);

protected:
private:

};

#endif