#include "WingsVolatilityModel.h"
#include "../dataobject/TemplateDO.h"

const std::string WingsParams::PARAM_F_ATM("f_atm");
const std::string WingsParams::PARAM_SSR("ssr");
const std::string WingsParams::PARAM_SCR("scr");
const std::string WingsParams::PARAM_F_REF("f_ref");
const std::string WingsParams::PARAM_VCR("vcr");
const std::string WingsParams::PARAM_VOL_REF("vol_ref");
const std::string WingsParams::PARAM_SLOPE_REF("slope_ref");
const std::string WingsParams::PARAM_DN_CF("dn_cf");
const std::string WingsParams::PARAM_UP_CF("up_cf");
const std::string WingsParams::PARAM_UP_SM("up_sm");
const std::string WingsParams::PARAM_DN_SM("dn_sm");
const std::string WingsParams::PARAM_PUT_CURV("put_curv");
const std::string WingsParams::PARAM_CALL_CURV("call_curv");
const std::string WingsParams::PARAM_DN_SLOPE("dn_slope");
const std::string WingsParams::PARAM_UP_SLOPE("up_slope");
const std::string WingsParams::PARAM_DAYS("days");
const std::string WingsParams::PARAM_ALPHA("alpha");

const std::string & WingsVolatilityModel::Name(void) const
{
	static const std::string ws("wings-volatility");
	return ws;
}

dataobj_ptr WingsVolatilityModel::Compute(
	const void* pInputObject,
	const ModelParamsDO& modelParams,
	const param_vector* params)
{
	WingsParams paramObj;
	if (!ParseParams(modelParams, &paramObj))
		return nullptr;

	auto& moneyVector = *(std::vector<double>*)pInputObject;

	// synthetic forward price
	double f_atm = paramObj.f_atm;
	double ssr = paramObj.ssr;
	double scr = paramObj.scr;
	double f_ref = paramObj.f_ref;
	double vcr = paramObj.vcr;
	double vol_ref = paramObj.vol_ref;
	double slope_ref = paramObj.slope_ref;
	double dn_cf = paramObj.dn_cf;
	double up_cf = paramObj.up_cf;
	double dn_sm = paramObj.dn_sm;
	double up_sm = paramObj.up_sm;
	double put_curv = paramObj.put_curv;
	double call_curv = paramObj.call_curv;
	double dn_slope = paramObj.dn_slope;
	double up_slope = paramObj.up_slope;
	double days = paramObj.days;
	double alpha = paramObj.alpha;

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

	auto ret = std::make_shared<VectorDO<double>>();

	for (double moneyness : moneyVector)
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

		ret->push_back(theo);
	}

	return ret;
}

const std::map<std::string, double>& WingsVolatilityModel::DefaultParams(void)
{
	static std::map<std::string, double> defaultParams = {
		{WingsParams::PARAM_F_ATM, 0},
		{WingsParams::PARAM_SSR,0},
		{WingsParams::PARAM_SCR, 0},
		{WingsParams::PARAM_F_REF, 0 },
		{WingsParams::PARAM_VCR, 0 },
		{WingsParams::PARAM_VOL_REF, 0 },
		{WingsParams::PARAM_SLOPE_REF, 0 },
		{WingsParams::PARAM_DN_CF, 0 },
		{WingsParams::PARAM_UP_CF, 0 },
		{WingsParams::PARAM_UP_SM, 0 },
		{WingsParams::PARAM_DN_SM, 0 },
		{WingsParams::PARAM_PUT_CURV, 0 },
		{WingsParams::PARAM_CALL_CURV, 0 },
		{WingsParams::PARAM_DN_SLOPE, 0 },
		{WingsParams::PARAM_UP_SLOPE, 0 },
		{WingsParams::PARAM_DAYS, 0 },
		{WingsParams::PARAM_ALPHA, 0 }
	};
	return defaultParams;
}

bool WingsVolatilityModel::ParseParams(const ModelParamsDO& modelParams, void * pParamObj)
{
	bool ret = true;
	WingsParams* pParams = (WingsParams*)pParamObj;
	pParams->f_atm = modelParams.ScalaParams.at(WingsParams::PARAM_F_ATM);
	pParams->ssr = modelParams.ScalaParams.at(WingsParams::PARAM_SSR);
	pParams->scr = modelParams.ScalaParams.at(WingsParams::PARAM_SCR);
	pParams->f_ref = modelParams.ScalaParams.at(WingsParams::PARAM_F_REF);
	pParams->vcr = modelParams.ScalaParams.at(WingsParams::PARAM_VCR);
	pParams->vol_ref = modelParams.ScalaParams.at(WingsParams::PARAM_VOL_REF);
	pParams->slope_ref = modelParams.ScalaParams.at(WingsParams::PARAM_SLOPE_REF);
	pParams->dn_cf = modelParams.ScalaParams.at(WingsParams::PARAM_DN_CF);
	pParams->up_cf = modelParams.ScalaParams.at(WingsParams::PARAM_UP_CF);
	pParams->dn_sm = modelParams.ScalaParams.at(WingsParams::PARAM_DN_SM);
	pParams->up_sm = modelParams.ScalaParams.at(WingsParams::PARAM_UP_SM);
	pParams->put_curv = modelParams.ScalaParams.at(WingsParams::PARAM_PUT_CURV);
	pParams->call_curv = modelParams.ScalaParams.at(WingsParams::PARAM_CALL_CURV);
	pParams->dn_slope = modelParams.ScalaParams.at(WingsParams::PARAM_DN_SLOPE);
	pParams->up_slope = modelParams.ScalaParams.at(WingsParams::PARAM_UP_SLOPE);
	pParams->days = modelParams.ScalaParams.at(WingsParams::PARAM_DAYS);
	pParams->alpha = modelParams.ScalaParams.at(WingsParams::PARAM_ALPHA);

	return ret;
}
