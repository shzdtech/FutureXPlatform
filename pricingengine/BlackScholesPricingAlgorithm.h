/***********************************************************************
 * Module:  BlackScholesPricingAlgorithm.h
 * Author:  milk
 * Modified: 2016年5月9日 22:57:32
 * Purpose: Declaration of the class BlackScholesPricingAlgorithm
 ***********************************************************************/

#if !defined(__pricingalgorithm_BlackScholesPricingAlgorithm_h)
#define __pricingalgorithm_BlackScholesPricingAlgorithm_h

#include "../pricingengine/IPricingAlgorithm.h"

class BlackScholesParams
{
public:
	static const std::string volatility_name;
	static const std::string spread_name;
	static const std::string riskFreeRate_name;

	double volatility;
	double riskFreeRate;
	double spread;
};


class BlackScholesPricingAlgorithm : public IPricingAlgorithm
{
public:
	virtual const std::string& Name(void) const;
	virtual std::shared_ptr<PricingDO> Compute(
		const void* pInputObject,
		const StrategyContractDO& sdo,
		IPricingDataContext& priceCtx,
		const param_vector* params);
	virtual const std::map<std::string, double>& DefaultParams(void);
	virtual bool ParseParams(const ModelParamsDO& modelParams, void* pParamObj);

protected:
	double ComputeOptionPrice(
		double underlyingPrice,
		const BlackScholesParams& params,
		const StrategyContractDO& sdo);

private:

};

#endif