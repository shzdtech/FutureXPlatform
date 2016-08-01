#include "WingsVolatilityModel.h"

static const std::string PARAM_F_ATM("f_atm");
static const std::string PARAM_SSR("ssr");
static const std::string PARAM_SCR("scr");
static const std::string PARAM_F_REF("f_ref");
static const std::string PARAM_VCR("vcr");
static const std::string PARAM_VOL_REF("vol_ref");
static const std::string PARAM_SLOPE_REF("slope_ref");
static const std::string PARAM_DN_CF("dn_cf");
static const std::string PARAM_UP_CF("up_cf");
static const std::string PARAM_UP_SM("up_sm");
static const std::string PARAM_DN_SM("dn_sm");
static const std::string PARAM_PUT_CURV("put_curv");
static const std::string PARAM_CALL_CURV("call_curv");
static const std::string PARAM_DN_SLOPE("dn_slope");
static const std::string PARAM_UP_SLOPE("up_slope");
static const std::string PARAM_DAYS("days");
static const std::string PARAM_ALPHA("alpha");

const std::string & WingsVolatilityModel::Name(void) const
{
	static const std::string ws("wings-volatility");
	return ws;
}

dataobj_ptr WingsVolatilityModel::Compute(const dataobj_ptr & input)
{
	auto& model = *(ModelParamsDO*)input.get();
	// synthetic forward price
	double f_atm = model.ScalaParams[PARAM_F_ATM];
	double ssr = model.ScalaParams[PARAM_SSR];
	double scr = model.ScalaParams[PARAM_SCR];
	double f_ref = model.ScalaParams[PARAM_F_REF];
	double vcr = model.ScalaParams[PARAM_VCR];
	double vol_ref = model.ScalaParams[PARAM_VOL_REF];
	double slope_ref = model.ScalaParams[PARAM_SLOPE_REF];
	double dn_cf = model.ScalaParams[PARAM_DN_CF];
	double up_cf = model.ScalaParams[PARAM_UP_CF];
	double dn_sm = model.ScalaParams[PARAM_DN_SM];
	double up_sm = model.ScalaParams[PARAM_UP_SM];
	double put_curv = model.ScalaParams[PARAM_PUT_CURV];
	double call_curv = model.ScalaParams[PARAM_CALL_CURV];
	double dn_slope = model.ScalaParams[PARAM_DN_SLOPE];
	double up_slope = model.ScalaParams[PARAM_UP_SLOPE];
	double days = model.ScalaParams[PARAM_DAYS];
	double alpha = model.ScalaParams[PARAM_ALPHA];

	double f_syn = std::pow(f_atm, (ssr / 100)) * std::pow(f_ref, (1 - f_ref / 100));
	// current volatility and current slope, i.e., when log - moneyness x = 0
	double vol_curr = vol_ref - vcr * ssr * (f_syn - f_ref) / f_ref;
	double slope_curr = slope_ref - scr * ssr * (f_syn - f_ref) / f_ref;

	// end points of the range
	double x1 = dn_cf;
	double x2 = up_cf;
	double x0 = (1 + dn_sm) * dn_cf;
	double x3 = (1 + up_sm) * up_cf;

	// "sigma_x1" is derived from put wing function 
	double sigma_x1 = vol_curr + slope_curr * x1 + put_curv * x1 * x1;
	double d_sigma_x1 = slope_curr + put_curv * x1;

	// "sigma_x0" is derived from down smoothing range function
	double dn_sm_c = (d_sigma_x1 - dn_slope) / (2 * (x1 - x0));
	double dn_sm_b = dn_slope - (d_sigma_x1 - dn_slope) * x0 / (x1 - x0);
	double dn_sm_a = sigma_x1 - dn_sm_b * x1 - dn_sm_c * x1 * x1;
	double sigma_x0 = dn_sm_a + dn_sm_b * x0 + dn_sm_c * x0 * x0;

	// "sigma_x2" is derived from call wing function 
	double sigma_x2 = vol_curr + slope_curr * x2 + call_curv * x2 * x2;
	double d_sigma_x2 = slope_curr + call_curv * x2;

	// "sigma_x3" is derived from up smoothing range function
	double up_sm_c = (d_sigma_x2 - up_slope) / (2 * (x2 - x3));
	double up_sm_b = up_slope - (d_sigma_x2 - up_slope) * x3 / (x2 - x3);
	double up_sm_a = sigma_x2 - up_sm_b * x2 - up_sm_c * x2 * x2;
	double sigma_x3 = up_sm_a + up_sm_b * x3 + up_sm_c * x3 * x3;

	auto ret = std::make_shared<ModelParamsDO>();
	ret->SerialId = model.SerialId;
	ret->ModelName = model.ModelName;

	for (double moneyness : model.Values)
	{
		// log - moneyness, i.e., transformed strike price
		double x = (1 / std::pow((days / 365), alpha)) * std::log(moneyness / f_syn);

		double theo = 0;
		// regions of the wing model
		if ((x1 <= x) & (x < 0)) {
			// 	put wing
			theo = vol_curr + slope_curr * x + put_curv * x * x;
		}
		else if ((0 <= x) & (x <= x2)) {
			// 	call wing
			theo = vol_curr + slope_curr * x + call_curv * x * x;
		}
		else if ((x0 <= x) & (x < x1)) {
			// 	down smoothing range
			theo = dn_sm_a + dn_sm_b * x + dn_sm_c * x * x;
		}
		else if ((x2 < x) & (x <= x3)) {
			// 	up smoothing range
			theo = up_sm_a + up_sm_b * x + up_sm_c * x * x;
		}
		else if (x < x0) {
			// 	down affine range
			theo = dn_slope * (x - x0) + sigma_x0;
		}
		else if (x3 < x) {
			// 	up affine range
			theo = up_slope * (x - x3) + sigma_x3;
		}

		ret->Values.push_back(theo);
	}

	return ret;
}
