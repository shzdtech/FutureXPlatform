#if !defined(__pricingengine_WingsVolatilityModel_h)
#define __pricingengine_WingsVolatilityModel_h

#include "IModelAlgorithm.h"

class WingsParams : public ParamsBase
{
public:
	static const std::string PARAM_ALPHA;
	static const std::string PARAM_F_REF;
	static const std::string PARAM_SSR;


	static const std::string PARAM_SCR;
	static const std::string PARAM_SCR_OFFSET;
	static const std::string PARAM_VCR;
	static const std::string PARAM_VCR_OFFSET;
	static const std::string PARAM_VOL_REF;
	static const std::string PARAM_VOL_REF_OFFSET;
	static const std::string PARAM_SLOPE_REF;
	static const std::string PARAM_SLOPE_REF_OFFSET;
	static const std::string PARAM_DN_CF;
	static const std::string PARAM_DN_CF_OFFSET;
	static const std::string PARAM_UP_CF;
	static const std::string PARAM_UP_CF_OFFSET;
	static const std::string PARAM_UP_SM;
	static const std::string PARAM_UP_SM_OFFSET;
	static const std::string PARAM_DN_SM;
	static const std::string PARAM_DN_SM_OFFSET;
	static const std::string PARAM_PUT_CURV;
	static const std::string PARAM_PUT_CURV_OFFSET;
	static const std::string PARAM_CALL_CURV;
	static const std::string PARAM_CALL_CURV_OFFSET;
	static const std::string PARAM_DN_SLOPE;
	static const std::string PARAM_DN_SLOPE_OFFSET;
	static const std::string PARAM_UP_SLOPE;
	static const std::string PARAM_UP_SLOPE_OFFSET;
	

public:
	double alpha;
	double f_ref;
	double ssr;

	double scr;
	double scr_offset;
	double vcr;
	double vcr_offset;
	double vol_ref;
	double vol_ref_offset;
	double slope_ref;
	double slope_ref_offset;
	double dn_cf;
	double dn_cf_offset;
	double up_cf;
	double up_cf_offset;
	double dn_sm;
	double dn_sm_offset;
	double up_sm;
	double up_sm_offset;
	double put_curv;
	double put_curv_offset;
	double call_curv;
	double call_curv_offset;
	double dn_slope;
	double dn_slope_offset;
	double up_slope;
	double up_slope_offset;
	
};


class WingsVolatilityModel : public IModelAlgorithm
{
public:
	virtual const std::string& Name(void) const;
	virtual dataobj_ptr Compute(
		const void* pInputObject,
		const StrategyContractDO& sdo,
		const IPricingDataContext_Ptr& priceCtx_Ptr,
		const param_vector* params);
	static std::tuple<double, double, double, double, double, double, double>
		ComputeVolatility(
		double f_syn,
		double x,
		double f_ref,
		double ssr,
		double scr,
		double vcr,
		double vol_ref,
		double slope_ref,
		double dn_cf,
		double up_cf,
		double dn_sm,
		double up_sm,
		double dn_slope,
		double up_slope,
		double put_curv,
		double call_curv
	);
	virtual const std::map<std::string, double>& DefaultParams(void) const;
	virtual void ParseParams(const std::map<std::string, double>& modelParams, std::unique_ptr<ParamsBase>& target);

private:
	static int _yearDays;
};

#endif

