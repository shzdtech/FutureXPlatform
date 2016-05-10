/***********************************************************************
 * Module:  BlackScholesPricing.cpp
 * Author:  milk
 * Modified: 2016年5月9日 22:57:32
 * Purpose: Implementation of the class BlackScholesPricing
 ***********************************************************************/

#include "BlackScholesPricing.h"
#include <ql/quantlib.hpp>

#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParam.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractDO.h"
#include "../dataobject/TypedefDO.h"

using namespace QuantLib;

////////////////////////////////////////////////////////////////////////
// Name:       BlackScholesPricing::Name()
// Purpose:    Implementation of BlackScholesPricing::Name()
// Return:     std::string
////////////////////////////////////////////////////////////////////////

const std::string& BlackScholesPricing::Name(void) const
{
	static const std::string bs("black-scholes");
	return bs;
}

////////////////////////////////////////////////////////////////////////
// Name:       BlackScholesPricing::Compute(ParamVector params)
// Purpose:    Implementation of BlackScholesPricing::Compute()
// Parameters:
// - params
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr BlackScholesPricing::Compute(
	const StrategyContractDO& sdo,
	double inputVal,
	PricingContext& priceCtx,
	const ParamVector* params) const
{
	dataobj_ptr ret;

	if (sdo.BaseContracts && sdo.BaseContracts->size() > 0)
	{
		auto& mdDOMap = *(priceCtx.GetMarketDataDOMap());
		auto& conDOMap = *(priceCtx.GetContractMap());

		auto& conparam = sdo.BaseContracts->at(0);
		auto& md = mdDOMap.at(conparam.InstrumentID());

		Calendar calendar = TARGET();
		Real underlying = md.LastPrice;
		Real strike = inputVal;
		Spread dividendYield = sdo.Spread;
		Rate riskFreeRate = sdo.RiskFreeRate;
		Volatility volatility = sdo.Volatility;
		DayCounter dayCounter = Actual365Fixed();

		Date maturity((Day)sdo.Muturity.tm_mday,
			(Month)sdo.Muturity.tm_mon,
			(Year)sdo.Muturity.tm_year);

		Date settlementDate((Day)sdo.SettlementDate.tm_mday,
			(Month)sdo.SettlementDate.tm_mon,
			(Year)sdo.SettlementDate.tm_year);

		Handle<Quote> underlyingH(
			boost::shared_ptr<Quote>(new SimpleQuote(strike)));

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

		boost::shared_ptr<StrikedTypePayoff> payoffPut(
			new PlainVanillaPayoff(Option::Put, strike));
		boost::shared_ptr<StrikedTypePayoff> payoffCall(
			new PlainVanillaPayoff(Option::Call, strike));

		boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(
			new BlackScholesMertonProcess(underlyingH, flatDividendTS,
				flatTermStructure, flatVolTS));

		boost::shared_ptr<PricingEngine> pricingEngine(new AnalyticEuropeanEngine(bsmProcess));

		// options
		boost::shared_ptr<Exercise> europeanExercise(
			new EuropeanExercise(maturity));

		VanillaOption europeanPutOption(payoffPut, europeanExercise);
		europeanPutOption.setPricingEngine(pricingEngine);

		VanillaOption europeanCallOption(payoffCall, europeanExercise);
		europeanPutOption.setPricingEngine(pricingEngine);

		PricingDO* pDO = new PricingDO(sdo.ExchangeID(), sdo.InstrumentID());
		ret.reset(pDO);


		pDO->BidPrice = europeanPutOption.NPV();
		pDO->AskPrice = europeanCallOption.NPV();
	}

	return ret;

}