/***********************************************************************
 * Module:  StrategyConstractDAO.h
 * Author:  milk
 * Modified: 2015年8月8日 22:30:32
 * Purpose: Declaration of the class StrategyConstractDAO
 ***********************************************************************/

#if !defined(__databaseop_StrategyContractDAO_h)
#define __databaseop_StrategyContractDAO_h

#include <map>
#include "../dataobject/ContractKey.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/TemplateDO.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS StrategyContractDAO
{
public:
	static VectorDO_Ptr<StrategyContractDO> FindStrategyContractByUser(const std::string& userid, int productType);
	static void RetrievePricingContracts(const std::string& strategySymbol, const std::string& userid, std::vector<PricingContract>& pricingContracts);
	static void RetrieveModelParams(const std::string& strategySymbol, const std::string& userid, ModelParamsDO& modelParams);

protected:
private:

};

#endif