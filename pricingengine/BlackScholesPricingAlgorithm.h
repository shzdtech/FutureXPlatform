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
	static const std::string spread_name;
	static const std::string riskFreeRate_name;

	double riskFreeRate;
	double spread;
};


class BlackScholesPricingAlgorithm : public IPricingAlgorithm
{
public:
	virtual const std::string& Name(void) const;
	virtual dataobj_ptr Compute(
		const void* pInputObject,
		const StrategyContractDO& sdo,
		IPricingDataContext& priceCtx,
		const param_vector* params);
	virtual const std::map<std::string, double>& DefaultParams(void);
	virtual bool ParseParams(const std::map<std::string, double>& params, void* pParamObj);

protected:
	double ComputeOptionPrice(
		const BlackScholesParams& params,
		double inputPrice,
		bool call,
		const StrategyContractDO& sdo,
		IPricingDataContext& priceCtx
		);

private:

};

#endif