/***********************************************************************
 * Module:  BlackScholesPricingAlgorithm.cpp
 * Author:  milk
 * Modified: 2016年5月9日 22:57:32
 * Purpose: Implementation of the class BlackScholesPricingAlgorithm
 ***********************************************************************/

#include "BlackScholesPricingAlgorithm.h"
#include <ql/quantlib.hpp>

#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractParamDO.h"
#include "../dataobject/TypedefDO.h"
#include "../bizutility/OptionPricingCache.h"

using namespace QuantLib;

const std::string BlackScholesParams::riskFreeRate_name("riskFreeRate");
const std::string BlackScholesParams::spread_name("spread");
////////////////////////////////////////////////////////////////////////
// Name:       BlackScholesPricingAlgorithm::Name()
// Purpose:    Implementation of BlackScholesPricingAlgorithm::Name()
// Return:     std::string
////////////////////////////////////////////////////////////////////////

const std::string& BlackScholesPricingAlgorithm::Name(void) const
{
	static const std::string bs("black-scholes");
	return bs;
}

////////////////////////////////////////////////////////////////////////
// Name:       BlackScholesPricingAlgorithm::Compute(param_vector params)
// Purpose:    Implementation of BlackScholesPricingAlgorithm::Compute()
// Parameters:
// - params
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr BlackScholesPricingAlgorithm::Compute(
	const void* pInputObject,
	const StrategyContractDO& sdo,
	IPricingDataContext& priceCtx,
	const param_vector* params)
{
	BlackScholesParams paramObj;
	if (!ParseParams(sdo.Params, &paramObj))
		return nullptr;

	double bidPrice = ((double*)pInputObject)[0];
	double askdPrice = ((double*)pInputObject)[1];

	PricingDO callPricingDO(sdo.ExchangeID(), sdo.InstrumentID());

	PricingDO putPricingDO(sdo.ExchangeID(), sdo.InstrumentID());

	if (!OptionPricingCache::FindCallOption(sdo, bidPrice, callPricingDO.BidPrice))
	{
		callPricingDO.BidPrice = ComputeOptionPrice(paramObj, bidPrice, true, sdo, priceCtx);
		OptionPricingCache::AddCallOption(sdo, bidPrice, callPricingDO.BidPrice);
	}

	if (!OptionPricingCache::FindCallOption(sdo, askdPrice, callPricingDO.AskPrice))
	{
		callPricingDO.AskPrice = ComputeOptionPrice(paramObj, askdPrice, true, sdo, priceCtx);
		OptionPricingCache::AddCallOption(sdo, askdPrice, callPricingDO.AskPrice);
	}

	if (!OptionPricingCache::FindPutOption(sdo, bidPrice, putPricingDO.BidPrice))
	{
		putPricingDO.BidPrice = ComputeOptionPrice(paramObj, bidPrice, false, sdo, priceCtx);
		OptionPricingCache::AddCallOption(sdo, bidPrice, putPricingDO.BidPrice);
	}

	if (!OptionPricingCache::FindPutOption(sdo, askdPrice, putPricingDO.AskPrice))
	{
		putPricingDO.AskPrice = ComputeOptionPrice(paramObj, askdPrice, false, sdo, priceCtx);
		OptionPricingCache::AddCallOption(sdo, askdPrice, putPricingDO.AskPrice);
	}

	auto ret = std::make_shared<VectorDO<PricingDO>>();
	ret->push_back(callPricingDO);
	ret->push_back(putPricingDO);

	return ret;
}

const std::map<std::string, double>& BlackScholesPricingAlgorithm::DefaultParams(void)
{
	static std::map<std::string, double> defaultParams = {
		{ BlackScholesParams::riskFreeRate_name , 0.05},
		{ BlackScholesParams::spread_name , 1 },
	};
	return defaultParams;
}

bool BlackScholesPricingAlgorithm::ParseParams(const std::map<std::string, double>& params, void * pParamObj)
{
	return false;
}

double BlackScholesPricingAlgorithm::ComputeOptionPrice(
	const BlackScholesParams& params,
	double inputPrice, bool call,
	const StrategyContractDO & sdo, IPricingDataContext & priceCtx)
{
	auto& mdDOMap = *(priceCtx.GetMarketDataMap());
	auto& conDOMap = *(priceCtx.GetContractParamMap());

	auto& baseContract = sdo.PricingContracts->at(0);
	auto& md = mdDOMap.at(baseContract.InstrumentID());

	Calendar calendar = TARGET();
	Real underlying = md.LastPrice;
	Spread dividendYield = params.spread;
	Rate riskFreeRate = params.riskFreeRate;
	Volatility volatility = sdo.Volatility;
	DayCounter dayCounter = Actual365Fixed();

	Date maturity((Day)sdo.Expiration.tm_mday,
		(Month)(sdo.Expiration.tm_mon + 1),
		(Year)(sdo.Expiration.tm_year + 1900));

	auto now = std::time(nullptr);
	auto pTM = std::localtime(&now);
	Date settlementDate((Day)pTM->tm_mday,
		(Month)(pTM->tm_mon + 1),
		(Year)(pTM->tm_year + 1900));

	Handle<Quote> underlyingH(
		boost::shared_ptr<Quote>(new SimpleQuote(underlying)));

	// bootstrap the yield/dividend/vol curves
	Handle<YieldTermStructure> flatTermStructure(
		boost::shared_ptr<YieldTermStructure>(
			new FlatForward(settlementDate, riskFreeRate, dayCounter)));
	Handle<YieldTermStructure> flatDividendTS(
		boost::shared_ptr<YieldTermStructure>(
			new FlatForward(settlementDate, dividendYield, dayCounter)));
	Handle<BlackVolTermStructure> flatVolTS(
		boost::shared_ptr<BlackVolTermStructure>(
			new BlackConstantVol(settlementDate, calendar, volatility,
				dayCounter)));

	boost::shared_ptr<StrikedTypePayoff> payoff(
		new PlainVanillaPayoff(call ? Option::Call : Option::Put, inputPrice));

	boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(
		new BlackScholesMertonProcess(underlyingH, flatDividendTS,
			flatTermStructure, flatVolTS));

	boost::shared_ptr<PricingEngine> pricingEngine(new AnalyticEuropeanEngine(bsmProcess));

	// options
	boost::shared_ptr<Exercise> europeanExercise(
		new EuropeanExercise(maturity));

	VanillaOption europeanOption(payoff, europeanExercise);
	europeanOption.setPricingEngine(pricingEngine);

	return europeanOption.NPV();
}
