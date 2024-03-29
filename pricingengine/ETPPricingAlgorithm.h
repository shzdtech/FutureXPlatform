/***********************************************************************
 * Module:  ETPPricingAlgorithm.h
 * Author:  milk
 * Modified: 2015年9月6日 23:23:15
 * Purpose: Declaration of the class ETPPricingAlgorithm
 ***********************************************************************/

#if !defined(__pricingengine_ETPPricingAlgorithm_h)
#define __pricingengine_ETPPricingAlgorithm_h

#include "IPricingAlgorithm.h"

class ETPParams : public ParamsBase
{
public:
	static const std::string coeff_name;
	static const std::string offset_name;
	static const std::string spread_name;

	double coeff;
	double offset;
	double spread;
};


class ETPPricingAlgorithm : public IPricingAlgorithm
{
public:
	virtual const std::string& Name(void) const;
	virtual IPricingDO_Ptr Compute(
		const void* pInputObject,
		const StrategyContractDO& sdo,
		const IPricingDataContext_Ptr& priceCtx_Ptr,
		const param_vector* params);
	virtual const std::map<std::string, double>& DefaultParams(void) const;
	virtual void ParseParams(const std::map<std::string, double>& modelParams, std::unique_ptr<ParamsBase>& target);

protected:
private:

};

#endif