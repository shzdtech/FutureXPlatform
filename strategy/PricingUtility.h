/***********************************************************************
 * Module:  PricingUtility.h
 * Author:  milk
 * Modified: 2015年10月15日 22:54:20
 * Purpose: Declaration of the class PricingUtility
 ***********************************************************************/

#if !defined(__strategy_PricingUtility_h)
#define __strategy_PricingUtility_h

#include "../dataobject/PricingDO.h"
#include "../dataobject/data_commondef.h"
#include "../dataobject/StrategyContractDO.h"
#include "strategy_exp.h"

class STRATEGY_CLASS_EXPORTS PricingUtility
{
public:
	static std::shared_ptr<PricingDO> Pricing(const ContractKey& strategy, const int quantity);
	static std::shared_ptr<PricingDO> Pricing(const StrategyContractDO& strategy, ParamVector& params);
	static std::shared_ptr<PricingDO> Pricing(const StrategyContractDO& strategy, const int quantity);

protected:
private:

};

#endif