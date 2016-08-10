#if !defined(__pricingengine_WingsVolatilityModel_h)
#define __pricingengine_WingsVolatilityModel_h

#include "IModelAlgorithm.h"

class WingsParams
{
public:
	static const std::string PARAM_F_ATM;
	static const std::string PARAM_SSR;
	static const std::string PARAM_SCR;
	static const std::string PARAM_F_REF;
	static const std::string PARAM_VCR;
	static const std::string PARAM_VOL_REF;
	static const std::string PARAM_SLOPE_REF;
	static const std::string PARAM_DN_CF;
	static const std::string PARAM_UP_CF;
	static const std::string PARAM_UP_SM;
	static const std::string PARAM_DN_SM;
	static const std::string PARAM_PUT_CURV;
	static const std::string PARAM_CALL_CURV;
	static const std::string PARAM_DN_SLOPE;
	static const std::string PARAM_UP_SLOPE;
	static const std::string PARAM_DAYS;
	static const std::string PARAM_ALPHA;

	double f_atm;
	double ssr;
	double scr;
	double f_ref;
	double vcr;
	double vol_ref;
	double slope_ref;
	double dn_cf;
	double up_cf;
	double dn_sm;
	double up_sm;
	double put_curv;
	double call_curv;
	double dn_slope;
	double up_slope;
	double days;
	double alpha;
};


class WingsVolatilityModel : public IModelAlgorithm
{
public:
	virtual const std::string& Name(void) const;
	virtual dataobj_ptr Compute(const dataobj_ptr& input);
	virtual const std::map<std::string, double>& DefaultParams(void);
	virtual bool ParseParams(const std::map<std::string, double>& params, void* pParamObj);
};

#endif

