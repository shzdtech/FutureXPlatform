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
	static VectorDO_Ptr<StrategyContractDO> FindStrategyContractByClient(const std::string& clientSymbol);
	static VectorDO_Ptr<PricingContract> FindPricingContracts(const std::string& strategySymbol, const std::string& clientSymbol);
	static void FindStrategyParams(const std::string& strategySymbol, const std::string& clientSymbol, std::map<std::string, double>& paramMap);

protected:
private:

};

#endif