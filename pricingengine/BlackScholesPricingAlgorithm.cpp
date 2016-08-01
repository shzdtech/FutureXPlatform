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
#include "../dataobject/ContractDO.h"
#include "../dataobject/TypedefDO.h"
#include "../bizutility/OptionPricingCache.h"

using namespace QuantLib;

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
	
	double bidPrice = ((double*)pInputObject)[0];
	double askdPrice = ((double*)pInputObject)[1];

	PricingDO callPricingDO(sdo.ExchangeID(), sdo.InstrumentID());
	callPricingDO.Type = 0;

	PricingDO putPricingDO(sdo.ExchangeID(), sdo.InstrumentID());
	putPricingDO.Type = 1;

	if (!OptionPricingCache::FindCallOption(sdo, bidPrice, callPricingDO.BidPrice))
	{
		callPricingDO.BidPrice = ComputeOptionPrice(bidPrice, callPricingDO.Type, sdo, priceCtx);
		OptionPricingCache::AddCallOption(sdo, bidPrice, callPricingDO.BidPrice);
	}

	if (!OptionPricingCache::FindCallOption(sdo, askdPrice, callPricingDO.AskPrice))
	{
		callPricingDO.AskPrice = ComputeOptionPrice(askdPrice, callPricingDO.Type, sdo, priceCtx);
		OptionPricingCache::AddCallOption(sdo, askdPrice, callPricingDO.AskPrice);
	}

	if (!OptionPricingCache::FindPutOption(sdo, bidPrice, putPricingDO.BidPrice))
	{
		putPricingDO.BidPrice = ComputeOptionPrice(bidPrice, putPricingDO.Type, sdo, priceCtx);
		OptionPricingCache::AddCallOption(sdo, bidPrice, putPricingDO.BidPrice);
	}

	if (!OptionPricingCache::FindPutOption(sdo, askdPrice, putPricingDO.AskPrice))
	{
		putPricingDO.AskPrice = ComputeOptionPrice(askdPrice, putPricingDO.Type, sdo, priceCtx);
		OptionPricingCache::AddCallOption(sdo, askdPrice, putPricingDO.AskPrice);
	}

	auto ret = std::make_shared<VectorDO<PricingDO>>();
	ret->push_back(callPricingDO);
	ret->push_back(putPricingDO);

	return ret;
}

double BlackScholesPricingAlgorithm::ComputeOptionPrice(double inputPrice, int optionType,
	const StrategyContractDO & sdo, IPricingDataContext & priceCtx)
{
	auto& mdDOMap = *(priceCtx.GetMarketDataDOMap());
	auto& conDOMap = *(priceCtx.GetContractMap());

	auto& baseContract = sdo.BaseContracts->at(0);
	auto& md = mdDOMap.at(baseContract.InstrumentID());

	Calendar calendar = TARGET();
	Real underlying = md.LastPrice;
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
		boost::shared_ptr<Quote>(new SimpleQuote(inputPrice)));

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

	boost::shared_ptr<StrikedTypePayoff> payoffP(
		new PlainVanillaPayoff(optionType > 0 ? Option::Put : Option::Call, inputPrice));

	boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(
		new BlackScholesMertonProcess(underlyingH, flatDividendTS,
			flatTermStructure, flatVolTS));

	boost::shared_ptr<PricingEngine> pricingEngine(new AnalyticEuropeanEngine(bsmProcess));

	// options
	boost::shared_ptr<Exercise> europeanExercise(
		new EuropeanExercise(maturity));

	VanillaOption europeanOption(payoffP, europeanExercise);
	europeanOption.setPricingEngine(pricingEngine);

	return europeanOption.NPV();
}
