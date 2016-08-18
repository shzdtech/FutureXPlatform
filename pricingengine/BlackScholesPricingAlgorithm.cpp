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

const std::string BlackScholesParams::volatility_name("volatility");
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

std::shared_ptr<PricingDO> BlackScholesPricingAlgorithm::Compute(
	const void* pInputObject,
	const StrategyContractDO& sdo,
	IPricingDataContext& priceCtx,
	const param_vector* params)
{
	BlackScholesParams paramObj;
	if (!ParseParams(sdo.ModelParams, &paramObj))
		return nullptr;

	MarketDataDO* pMdDO = (MarketDataDO*)pInputObject;
	double bidPrice = pMdDO->BidPrice;
	double askdPrice = pMdDO->AskPrice;

	auto pricingDO = std::make_shared<PricingDO>(sdo.ExchangeID(), sdo.InstrumentID());

	auto& pairObj = OptionPricingCache::FindOption(sdo, bidPrice);
	if (pairObj.first == paramObj.volatility)
		pricingDO->BidPrice = pairObj.second;
	else
	{ 
		pricingDO->BidPrice = std::floor(ComputeOptionPrice(bidPrice, paramObj, sdo) / sdo.TickSize) * sdo.TickSize;
		pairObj.first = paramObj.volatility;
		pairObj.second = pricingDO->BidPrice;
	}
	
	pairObj = OptionPricingCache::FindOption(sdo, askdPrice);
	if (pairObj.first == paramObj.volatility)
		pricingDO->AskPrice = pairObj.second;
	else
	{
		pricingDO->AskPrice = std::ceil(ComputeOptionPrice(askdPrice, paramObj, sdo) / sdo.TickSize) * sdo.TickSize;
		pairObj.first = paramObj.volatility;
		pairObj.second = pricingDO->AskPrice;
	}

	return pricingDO;
}

const std::map<std::string, double>& BlackScholesPricingAlgorithm::DefaultParams(void)
{
	static std::map<std::string, double> defaultParams = {
		{ BlackScholesParams::volatility_name , 0.2 },
		{ BlackScholesParams::riskFreeRate_name , 0.05},
		{ BlackScholesParams::spread_name , 1 },
	};
	return defaultParams;
}

bool BlackScholesPricingAlgorithm::ParseParams(const ModelParamsDO& modelParams, void * pParamObj)
{
	bool ret = true;

	BlackScholesParams* pParams = (BlackScholesParams*)pParamObj;
	pParams->volatility = modelParams.ScalaParams.at(BlackScholesParams::volatility_name);
	pParams->riskFreeRate = modelParams.ScalaParams.at(BlackScholesParams::riskFreeRate_name);
	pParams->spread = modelParams.ScalaParams.at(BlackScholesParams::spread_name);

	return ret;
}

double BlackScholesPricingAlgorithm::ComputeOptionPrice(
	double underlyingPrice,
	const BlackScholesParams& params,
	const StrategyContractDO& sdo)
{
	Calendar calendar = TARGET();
	Spread dividendYield = params.spread;
	Rate riskFreeRate = params.riskFreeRate;
	Volatility volatility = params.volatility;
	DayCounter dayCounter = Actual365Fixed();

	Date maturity((Day)sdo.Expiration.Day,
		(Month)(sdo.Expiration.Month),
		(Year)(sdo.Expiration.Year));

	Date settlementDate((Day)sdo.TradingDay.Day,
		(Month)(sdo.TradingDay.Month),
		(Year)(sdo.TradingDay.Year));

	Handle<Quote> underlyingH(
		boost::shared_ptr<Quote>(new SimpleQuote(underlyingPrice)));

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
		new PlainVanillaPayoff(
			sdo.ContractType == ContractType::CONTRACTTYPE_CALL_OPTION ? Option::Call : Option::Put, sdo.StrikePrice));

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
