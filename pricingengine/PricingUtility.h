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
#include "../utility/singleton_templ.h"
#include "PricingContext.h"

#include "pricingengine_exp.h"

class PRICINGENGINE_CLASS_EXPORTS PricingUtility
{
public:
	static std::shared_ptr<PricingDO> Pricing(
		const StrategyContractDO& strategy,
		double inputVal,
		PricingContext* priceCtx,
		const ParamVector* params);

	static std::shared_ptr<PricingDO> Pricing(
		const StrategyContractDO& strategy,
		double inputVal,
		PricingContext* priceCtx = nullptr);

	static std::shared_ptr<PricingDO> Pricing(
		const ContractKey& contractKey,
		double inputVal,
		PricingContext* priceCtx,
		const ParamVector* params);

	static std::shared_ptr<PricingDO> Pricing(
		const ContractKey& contractKey,
		double inputVal,
		PricingContext* priceCtx);


	static std::shared_ptr<PricingDO> Pricing(
		const StrategyContractDO& strategy,
		double inputVal,
		const ParamVector* params);

protected:
private:

};

#endif