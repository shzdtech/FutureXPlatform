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
#include "../bizutility/GlobalSettings.h"

using namespace QuantLib;

////////////////////////////////////////////////////////////////////////
// Name:       BlackScholesPricingAlgorithm::Name()
// Purpose:    Implementation of BlackScholesPricingAlgorithm::Name()
// Return:     std::string
////////////////////////////////////////////////////////////////////////

const std::string& BlackScholesPricingAlgorithm::Name(void) const
{
	static const std::string bs("bsm");
	return bs;
}

////////////////////////////////////////////////////////////////////////
// Name:       BlackScholesPricingAlgorithm::Compute(param_vector params)
// Purpose:    Implementation of BlackScholesPricingAlgorithm::Compute()
// Parameters:
// - params
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

IPricingDO_Ptr BlackScholesPricingAlgorithm::Compute(
	const void* pInputObject,
	const StrategyContractDO& sdo,
	IPricingDataContext& priceCtx,
	const param_vector* params)
{
	if (!sdo.PricingModel->ParsedParams)
	{
		ParseParams(sdo.PricingModel->Params, sdo.PricingModel->ParsedParams);
	}

	auto paramObj = (OptionParams*)sdo.PricingModel->ParsedParams.get();

	if (!sdo.PricingContracts)
		return nullptr;

	MarketDataDO mDO;
	if (!priceCtx.GetMarketDataMap()->find(sdo.PricingContracts->PricingContracts[0].InstrumentID(), mDO))
		return nullptr;

	double adjust = sdo.PricingContracts->PricingContracts[0].Adjust;

	double bidPrice = mDO.Bid().Price + adjust;
	double askdPrice = mDO.Ask().Price + adjust;

	if (bidPrice <= 0 || askdPrice <= 0)
		return nullptr;

	if (sdo.ContractType == ContractType::CONTRACTTYPE_PUT_OPTION) std::swap(bidPrice, askdPrice);

	auto pricingDO = std::make_shared<OptionPricingDO>(sdo.ExchangeID(), sdo.InstrumentID());

	ComputeOptionPrice(bidPrice, sdo.StrikePrice, paramObj->bidVolatility, paramObj->riskFreeRate, paramObj->dividend, sdo.ContractType, sdo.TradingDay, sdo.Expiration, pricingDO->TBid());
	pricingDO->TBid().Price = std::floor( std::max(0.0, pricingDO->TBid().Price) / sdo.TickSize) * sdo.TickSize;
	
	ComputeOptionPrice(askdPrice, sdo.StrikePrice, paramObj->askVolatility, paramObj->riskFreeRate, paramObj->dividend, sdo.ContractType, sdo.TradingDay, sdo.Expiration, pricingDO->TAsk());
	pricingDO->TAsk().Price = std::ceil(pricingDO->TAsk().Price / sdo.TickSize) * sdo.TickSize;
	
	return pricingDO;
}

const std::map<std::string, double>& BlackScholesPricingAlgorithm::DefaultParams(void) const
{
	static std::map<std::string, double> defaultParams = {
		{ OptionParams::riskFreeRate_name , GlobalSettings::RiskFreeRate() },
		{ OptionParams::dividend_name , 0 },
	};
	return defaultParams;
}

void BlackScholesPricingAlgorithm::ParseParams(const std::map<std::string, double>& modelParams, std::unique_ptr<ParamsBase>& target)
{
	auto ret = std::make_unique<OptionParams>();

	ret->riskFreeRate = modelParams.at(OptionParams::riskFreeRate_name);
	ret->dividend = modelParams.at(OptionParams::dividend_name);

	target = std::move(ret);
}


void BlackScholesPricingAlgorithm::ComputeOptionPrice(
	double underlyingPrice,
	double strikePrice,
	double volatility,
	double riskFreeRate,
	double dividendYield,
	ContractType contractType,
	const DateType& tradingDate,
	const DateType& maturityDate,
	OptionPricing & option)
{
	DayCounter dayCounter = Actual365Fixed();

	Date maturity((Day)maturityDate.Day, (Month)(maturityDate.Month), (Year)(maturityDate.Year));

	Date settlementDate((Day)tradingDate.Day, (Month)(tradingDate.Month), (Year)(tradingDate.Year));

	Option::Type optionType = contractType == ContractType::CONTRACTTYPE_CALL_OPTION ? Option::Call : Option::Put;

	Handle<Quote> underlyingH(boost::shared_ptr<Quote>(new SimpleQuote(underlyingPrice)));

	// bootstrap the yield/dividend/vol curves
	Handle<YieldTermStructure> flatTermStructure(
		boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, riskFreeRate, dayCounter)));
	Handle<YieldTermStructure> flatDividendTS(
		boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, dividendYield, dayCounter)));
	Handle<BlackVolTermStructure> flatVolTS(
		boost::shared_ptr<BlackVolTermStructure>(new BlackConstantVol(settlementDate, TARGET(), volatility, dayCounter)));

	boost::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(optionType, strikePrice));

	boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(new BlackScholesMertonProcess(underlyingH, flatDividendTS, flatTermStructure, flatVolTS));

	boost::shared_ptr<PricingEngine> pricingEngine(new AnalyticEuropeanEngine(bsmProcess));

	// options
	boost::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(maturity));

	VanillaOption europeanOption(payoff, europeanExercise);
	europeanOption.setPricingEngine(pricingEngine);

	option.Price = europeanOption.NPV();
	option.Delta = europeanOption.delta();
	option.Vega = europeanOption.vega();
	option.Gamma = europeanOption.gamma();
	option.Theta = europeanOption.theta();
}
