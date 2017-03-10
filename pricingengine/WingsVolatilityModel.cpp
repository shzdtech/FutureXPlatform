#include "WingsVolatilityModel.h"
#include <boost/date_time/gregorian/greg_date.hpp>
#include "../dataobject/TemplateDO.h"
#include "../dataobject/WingsModelReturnDO.h"

const std::string WingsParams::PARAM_ALPHA("alpha");
const std::string WingsParams::PARAM_SSR("ssr");
const std::string WingsParams::PARAM_F_REF("f_ref");

const std::string WingsParams::PARAM_SCR("scr");
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

const std::string WingsParams::PARAM_SCR_OFFSET("scr_offset");
const std::string WingsParams::PARAM_VCR_OFFSET("vcr_offset");
const std::string WingsParams::PARAM_VOL_REF_OFFSET("vol_ref_offset");
const std::string WingsParams::PARAM_SLOPE_REF_OFFSET("slope_ref_offset");
const std::string WingsParams::PARAM_DN_CF_OFFSET("dn_cf_offset");
const std::string WingsParams::PARAM_UP_CF_OFFSET("up_cf_offset");
const std::string WingsParams::PARAM_UP_SM_OFFSET("up_sm_offset");
const std::string WingsParams::PARAM_DN_SM_OFFSET("dn_sm_offset");
const std::string WingsParams::PARAM_PUT_CURV_OFFSET("put_curv_offset");
const std::string WingsParams::PARAM_CALL_CURV_OFFSET("call_curv_offset");
const std::string WingsParams::PARAM_DN_SLOPE_OFFSET("dn_slope_offset");
const std::string WingsParams::PARAM_UP_SLOPE_OFFSET("up_slope_offset");



const std::string & WingsVolatilityModel::Name(void) const
{
	static const std::string ws("wing");
	return ws;
}

dataobj_ptr WingsVolatilityModel::Compute(
	const void* pInputObject,
	const StrategyContractDO& sdo,
	IPricingDataContext& priceCtx,
	const param_vector* params)
{
	if (!sdo.VolModel->ParsedParams)
	{
		ParseParams(sdo.VolModel->Params, sdo.VolModel->ParsedParams);
	}

	auto paramObj = (WingsParams*)sdo.VolModel->ParsedParams.get();
	double f_atm;

	if (pInputObject)
	{
		f_atm = *(double*)pInputObject;
	}
	else
	{
		MarketDataDO mDO;
		if (!sdo.VolContracts || 
			!priceCtx.GetMarketDataMap()->find(sdo.VolContracts->PricingContracts[0].InstrumentID(), mDO))
			return nullptr;
		f_atm = (mDO.Ask().Price + mDO.Bid().Price) / 2;
		if (f_atm == 0)
			return nullptr;
	}

	auto strikeprice = sdo.StrikePrice;

	boost::gregorian::date settleDate(sdo.TradingDay.Year, sdo.TradingDay.Month, sdo.TradingDay.Day);
	boost::gregorian::date mutureDate(sdo.Expiration.Year, sdo.Expiration.Month, sdo.Expiration.Day);
	int days = (mutureDate - settleDate).days();

	if (days < 0)
		return nullptr;

	// synthetic forward price
	double f_ref = paramObj->f_ref;
	double ssr = paramObj->ssr;
	double alpha = paramObj->alpha;
	if (f_ref < 0.001) f_ref = f_atm;

	auto ret = std::make_shared<WingsModelReturnDO>();

	////synthetic forward price
	//  f_syn = f_atm ^ (SSR / 100) * f_ref ^ (1 - SSR / 100)	
	ret->f_syn = std::pow(f_atm, (paramObj->ssr / 100)) * std::pow(f_ref, (1 - ssr / 100));

	// log - moneyness, i.e., transformed strike price
	ret->x = (1 / std::pow((days / 365), alpha)) * std::log(strikeprice / ret->f_syn);

	double midVol;
	std::tie(midVol, ret->vol_curr, ret->slope_curr, ret->x0, ret->x1, ret->x2, ret->x3)
		= ComputeVolatility(ret->f_syn, ret->x, f_ref, alpha, ssr,
		paramObj->scr, paramObj->vcr, paramObj->vol_ref, paramObj->slope_ref, paramObj->dn_cf, paramObj->up_cf, paramObj->dn_sm, paramObj->up_sm, paramObj->dn_slope, paramObj->up_slope, paramObj->put_curv, paramObj->call_curv);

	double offsetVol;
	std::tie(offsetVol, ret->vol_curr_offset, ret->slope_curr_offset, ret->x0_offset, ret->x1_offset, ret->x2_offset, ret->x3_offset)
		= ComputeVolatility(ret->f_syn, ret->x, f_ref, alpha, ssr,
		paramObj->scr_offset, paramObj->vcr_offset, paramObj->vol_ref_offset, paramObj->slope_ref_offset, paramObj->dn_cf_offset, paramObj->up_cf_offset, paramObj->dn_sm_offset, paramObj->up_sm_offset, paramObj->dn_slope_offset, paramObj->up_slope_offset, paramObj->put_curv_offset, paramObj->call_curv_offset);

	ret->TBid().Volatility = midVol - offsetVol;
	ret->TAsk().Volatility = midVol + offsetVol;

	return ret;
}

std::tuple<double, double, double, double, double, double, double>
	WingsVolatilityModel::ComputeVolatility(
	double f_syn,
	double x,
	double f_ref,
	double alpha,
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
)
{
	////current volatility and current slope, i.e., when log - moneyness x = 0
	//  vol_curr = vol_ref - VCR * SSR * (f_syn - f_ref) / f_ref
	// slope_curr = slope_ref - SCR * SSR * (f_syn - f_ref) / f_ref
	double vol_curr = vol_ref - vcr * ssr * (f_syn - f_ref) / f_ref;
	double slope_curr = slope_ref - scr * ssr * (f_syn - f_ref) / f_ref;

	// end points of the range
	double x1 = dn_cf;
	double x2 = up_cf;
	double x0 = (1 + dn_sm) * dn_cf;
	double x3 = (1 + up_sm) * up_cf;

	//// "sigma_x1" is derived from put wing function
	//   sigma_x1 = vol_curr + slope_curr * x1 + put_curv * x1 ^ 2
	//	 d_sigma_x1 = slope_curr + put_curv * x1
	double sigma_x1 = vol_curr + slope_curr * x1 + put_curv * x1 * x1;
	double d_sigma_x1 = slope_curr + put_curv * x1;

	//// "sigma_x0" is derived from down smoothing range function
	//   dn_sm_c = (d_sigma_x1 - dn_slope) / (2 * (x1 - x0))
	//	 dn_sm_b = dn_slope - (d_sigma_x1 - dn_slope) * x0 / (x1 - x0)
	//	 dn_sm_a = sigma_x1 - dn_sm_b * x1 - dn_sm_c * x1 ^ 2
	//	 sigma_x0 = dn_sm_a + dn_sm_b * x0 + dn_sm_c * x0 ^ 2
	double dn_sm_c = (d_sigma_x1 - dn_slope) / (2 * (x1 - x0));
	double dn_sm_b = dn_slope - (d_sigma_x1 - dn_slope) * x0 / (x1 - x0);
	double dn_sm_a = sigma_x1 - dn_sm_b * x1 - dn_sm_c * x1 * x1;
	double sigma_x0 = dn_sm_a + dn_sm_b * x0 + dn_sm_c * x0 * x0;

	//// sigma_x2" is derived from call wing function 
	//   sigma_x2 = vol_curr + slope_curr * x2 + call_curv * x2 ^ 2
	//   d_sigma_x2 = slope_curr + call_curv * x2
	double sigma_x2 = vol_curr + slope_curr * x2 + call_curv * x2 * x2;
	double d_sigma_x2 = slope_curr + call_curv * x2;

	//// "sigma_x3" is derived from up smoothing range function
	//  up_sm_c = (d_sigma_x2 - up_slope) / (2 * (x2 - x3))
	//	up_sm_b = up_slope - (d_sigma_x2 - up_slope) * x3 / (x2 - x3)
	//	up_sm_a = sigma_x2 - up_sm_b * x2 - up_sm_c * x2 ^ 2
	//	sigma_x3 = up_sm_a + up_sm_b * x3 + up_sm_c * x3 ^ 2
	double up_sm_c = (d_sigma_x2 - up_slope) / (2 * (x2 - x3));
	double up_sm_b = up_slope - (d_sigma_x2 - up_slope) * x3 / (x2 - x3);
	double up_sm_a = sigma_x2 - up_sm_b * x2 - up_sm_c * x2 * x2;
	double sigma_x3 = up_sm_a + up_sm_b * x3 + up_sm_c * x3 * x3;

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

	return std::make_tuple(theo, vol_curr, slope_curr,  x0, x1, x2, x3);
}

const std::map<std::string, double>& WingsVolatilityModel::DefaultParams(void) const
{
	static std::map<std::string, double> defaultParams = {
		{WingsParams::PARAM_ALPHA, 0 },
		{WingsParams::PARAM_SSR,0},
		{WingsParams::PARAM_F_REF, 0 },

		{WingsParams::PARAM_SCR, 0 },
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

		{ WingsParams::PARAM_SCR_OFFSET, 0 },
		{ WingsParams::PARAM_VCR_OFFSET, 0 },
		{ WingsParams::PARAM_VOL_REF_OFFSET, 0 },
		{ WingsParams::PARAM_SLOPE_REF_OFFSET, 0 },
		{ WingsParams::PARAM_DN_CF_OFFSET, 0 },
		{ WingsParams::PARAM_UP_CF_OFFSET, 0 },
		{ WingsParams::PARAM_UP_SM_OFFSET, 0 },
		{ WingsParams::PARAM_DN_SM_OFFSET, 0 },
		{ WingsParams::PARAM_PUT_CURV_OFFSET, 0 },
		{ WingsParams::PARAM_CALL_CURV_OFFSET, 0 },
		{ WingsParams::PARAM_DN_SLOPE_OFFSET, 0 },
		{ WingsParams::PARAM_UP_SLOPE_OFFSET, 0 },

	};
	return defaultParams;
}

void WingsVolatilityModel::ParseParams(const std::map<std::string, double>& modelParams, std::unique_ptr<ParamsBase>& target)
{
	auto ret = std::make_unique<WingsParams>();
	ret->alpha = modelParams.at(WingsParams::PARAM_ALPHA);
	ret->ssr = modelParams.at(WingsParams::PARAM_SSR);
	ret->f_ref = modelParams.at(WingsParams::PARAM_F_REF);

	ret->scr = modelParams.at(WingsParams::PARAM_SCR);
	ret->vcr = modelParams.at(WingsParams::PARAM_VCR);
	ret->vol_ref = modelParams.at(WingsParams::PARAM_VOL_REF);
	ret->slope_ref = modelParams.at(WingsParams::PARAM_SLOPE_REF);
	ret->dn_cf = modelParams.at(WingsParams::PARAM_DN_CF);
	ret->up_cf = modelParams.at(WingsParams::PARAM_UP_CF);
	ret->dn_sm = modelParams.at(WingsParams::PARAM_DN_SM);
	ret->up_sm = modelParams.at(WingsParams::PARAM_UP_SM);
	ret->put_curv = modelParams.at(WingsParams::PARAM_PUT_CURV);
	ret->call_curv = modelParams.at(WingsParams::PARAM_CALL_CURV);
	ret->dn_slope = modelParams.at(WingsParams::PARAM_DN_SLOPE);
	ret->up_slope = modelParams.at(WingsParams::PARAM_UP_SLOPE);

	ret->scr_offset = modelParams.at(WingsParams::PARAM_SCR_OFFSET);
	ret->vcr_offset = modelParams.at(WingsParams::PARAM_VCR_OFFSET);
	ret->vol_ref_offset = modelParams.at(WingsParams::PARAM_VOL_REF_OFFSET);
	ret->slope_ref_offset = modelParams.at(WingsParams::PARAM_SLOPE_REF_OFFSET);
	ret->dn_cf_offset = modelParams.at(WingsParams::PARAM_DN_CF_OFFSET);
	ret->up_cf_offset = modelParams.at(WingsParams::PARAM_UP_CF_OFFSET);
	ret->dn_sm_offset = modelParams.at(WingsParams::PARAM_DN_SM_OFFSET);
	ret->up_sm_offset = modelParams.at(WingsParams::PARAM_UP_SM_OFFSET);
	ret->put_curv_offset = modelParams.at(WingsParams::PARAM_PUT_CURV_OFFSET);
	ret->call_curv_offset = modelParams.at(WingsParams::PARAM_CALL_CURV_OFFSET);
	ret->dn_slope_offset = modelParams.at(WingsParams::PARAM_DN_SLOPE_OFFSET);
	ret->up_slope_offset = modelParams.at(WingsParams::PARAM_UP_SLOPE_OFFSET);

	target = std::move(ret);
}
