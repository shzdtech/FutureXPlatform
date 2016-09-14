/***********************************************************************
 * Module:  BetaSumPricingAlgorithm.h
 * Author:  milk
 * Modified: 2015年8月9日 0:42:12
 * Purpose: Declaration of the class BetaSumPricingAlgorithm
 ***********************************************************************/

#if !defined(__pricingengine_BetaSumPricingAlgorithm_h)
#define __pricingengine_BetaSumPricingAlgorithm_h

#include "IPricingAlgorithm.h"

class BetaSumParams
{
public:
	static const std::string offset_name;
	static const std::string spread_name;

	double offset;
	double spread;
};


class BetaSumPricingAlgorithm : public IPricingAlgorithm
{
public:
	virtual const std::string& Name(void) const;
	virtual IPricingDO_Ptr Compute(
		const void* pInputObject,
		const StrategyContractDO& sdo,
		IPricingDataContext& priceCtx,
		const param_vector* params);
	virtual const std::map<std::string, double>& DefaultParams(void) const;
	virtual std::shared_ptr<void> ParseParams(const std::map<std::string, double>& modelParams);

protected:
private:

};

#endif