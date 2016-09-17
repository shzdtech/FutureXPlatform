/***********************************************************************
 * Module:  BlackScholesIVM.cpp
 * Author:  milk
 * Modified: 2016年5月9日 22:57:32
 * Purpose: Implementation of the class BlackScholesIVM
 ***********************************************************************/

#include "BlackScholesIVM.h"
#include <ql/quantlib.hpp>

#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractParamDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TemplateDO.h"
#include "../bizutility/GlobalSettings.h"

using namespace QuantLib;

////////////////////////////////////////////////////////////////////////
// Name:       BlackScholesIVM::Name()
// Purpose:    Implementation of BlackScholesIVM::Name()
// Return:     std::string
////////////////////////////////////////////////////////////////////////

const std::string& BlackScholesIVM::Name(void) const
{
	static const std::string bs("bsivm");
	return bs;
}

////////////////////////////////////////////////////////////////////////
// Name:       BlackScholesIVM::Compute(param_vector params)
// Purpose:    Implementation of BlackScholesIVM::Compute()
// Parameters:
// - params
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr BlackScholesIVM::Compute(
	const void* pInputObject,
	const StrategyContractDO& sdo,
	IPricingDataContext& priceCtx,
	const param_vector* params)
{
	if (!sdo.PricingModel && !sdo.PricingModel->ParsedParams)
		return nullptr;

	auto paramObj = (OptionParams*)sdo.VolModel->ParsedParams.get();

	auto& mdo = priceCtx.GetMarketDataMap()->at(sdo.InstrumentID());
	auto& base_mdo = priceCtx.GetMarketDataMap()->at(sdo.PricingContracts[0].InstrumentID());

	double bidPrice = mdo.Bid().Price;
	double askdPrice = mdo.Ask().Price;

	double base_bidPrice = base_mdo.Bid().Price + paramObj->atmOffset;
	double base_askdPrice = base_mdo.Ask().Price + paramObj->atmOffset;

	auto ret = std::make_shared<TDataObject<std::pair<double, double>>>();

	ret->Data.first = ImpliedVolatility(bidPrice, base_bidPrice, sdo.StrikePrice, paramObj->bidVolatility, paramObj->riskFreeRate, paramObj->dividend, sdo.ContractType, sdo.TradingDay, sdo.Expiration);
	ret->Data.second = ImpliedVolatility(askdPrice, base_askdPrice, sdo.StrikePrice, paramObj->askVolatility, paramObj->riskFreeRate, paramObj->dividend, sdo.ContractType, sdo.TradingDay, sdo.Expiration);

	return ret;
}

const std::map<std::string, double>& BlackScholesIVM::DefaultParams(void) const
{
	static std::map<std::string, double> defaultParams = {
		{ OptionParams::riskFreeRate_name , GlobalSettings::RiskFreeRate() },
		{ OptionParams::dividend_name , 0 },
		{ OptionParams::atmOffset_name, 0 },
	};
	return defaultParams;
}

void BlackScholesIVM::ParseParams(const std::map<std::string, double>& modelParams, std::unique_ptr<ParamsBase>& target)
{
	auto ret = std::make_unique<OptionParams>();

	ret->riskFreeRate = modelParams.at(OptionParams::riskFreeRate_name);
	ret->dividend = modelParams.at(OptionParams::dividend_name);
	ret->atmOffset = modelParams.at(OptionParams::atmOffset_name);

	target = std::move(ret);
}

double BlackScholesIVM::ImpliedVolatility(
	double marketOptionPrice,
	double underlyingPrice,
	double strikePrice,
	double volatility,
	double riskFreeRate,
	double dividendYield,
	ContractType contractType,
	const DateType& tradingDate,
	const DateType& maturityDate)
{
	Calendar calendar = TARGET();
	DayCounter dayCounter = Actual365Fixed();

	Date maturity((Day)maturityDate.Day, (Month)(maturityDate.Month), (Year)(maturityDate.Year));

	Date settlementDate((Day)tradingDate.Day, (Month)(tradingDate.Month), (Year)(tradingDate.Year));

	Handle<Quote> underlyingH(boost::shared_ptr<Quote>(new SimpleQuote(underlyingPrice)));

	// bootstrap the yield/dividend/vol curves
	Handle<YieldTermStructure> flatTermStructure(
		boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, riskFreeRate, dayCounter)));
	Handle<YieldTermStructure> flatDividendTS(
		boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, dividendYield, dayCounter)));
	Handle<BlackVolTermStructure> flatVolTS(
		boost::shared_ptr<BlackVolTermStructure>(new BlackConstantVol(settlementDate, calendar, volatility, dayCounter)));

	boost::shared_ptr<StrikedTypePayoff> payoff(
		new PlainVanillaPayoff(contractType == ContractType::CONTRACTTYPE_CALL_OPTION ? Option::Call : Option::Put, strikePrice));

	boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(
		new BlackScholesMertonProcess(underlyingH, flatDividendTS, flatTermStructure, flatVolTS));

	boost::shared_ptr<PricingEngine> pricingEngine(new AnalyticEuropeanEngine(bsmProcess));

	// options
	boost::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(maturity));

	VanillaOption europeanOption(payoff, europeanExercise);
	europeanOption.setPricingEngine(pricingEngine);

	return europeanOption.impliedVolatility(marketOptionPrice, bsmProcess);
}