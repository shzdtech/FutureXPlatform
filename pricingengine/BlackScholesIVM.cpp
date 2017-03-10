/***********************************************************************
 * Module:  BlackScholesIVM.cpp
 * Author:  milk
 * Modified: 2016年5月9日 22:57:32
 * Purpose: Implementation of the class BlackScholesIVM
 ***********************************************************************/

#include "BlackScholesIVM.h"
#include <ql/pricingengines/blackcalculator.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/math/solvers1d/brent.hpp>

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
	if (!sdo.IVModel->ParsedParams)
	{
		ParseParams(sdo.IVModel->Params, sdo.IVModel->ParsedParams);
	}

	auto paramObj = (OptionParams*)sdo.IVModel->ParsedParams.get();

	if (!sdo.IVMContracts)
		return nullptr;

	MarketDataDO mDO;
	if (!priceCtx.GetMarketDataMap()->find(sdo.InstrumentID(), mDO))
		return nullptr;

	MarketDataDO mBaseDO;
	if (!priceCtx.GetMarketDataMap()->find(sdo.IVMContracts->PricingContracts[0].InstrumentID(), mBaseDO))
		return nullptr;

	double adjust = sdo.IVMContracts->PricingContracts[0].Adjust;

	double price = (mDO.Bid().Price + mDO.Ask().Price) / 2;

	double base_bidPrice = mBaseDO.Bid().Price + adjust;
	double base_askdPrice = mBaseDO.Ask().Price + adjust;

	if (price <= 0 || base_bidPrice <= 0 || base_askdPrice <= 0)
	{
		return nullptr;
	}

	auto ret = std::make_shared<TDataObject<std::pair<double, double>>>();

	try
	{
		ret->Data.first = ImpliedVolatility(price, base_bidPrice, sdo.StrikePrice, paramObj->bidVolatility, paramObj->riskFreeRate, paramObj->dividend, sdo.ContractType, sdo.TradingDay, sdo.Expiration);
		ret->Data.second = ImpliedVolatility(price, base_askdPrice, sdo.StrikePrice, paramObj->askVolatility, paramObj->riskFreeRate, paramObj->dividend, sdo.ContractType, sdo.TradingDay, sdo.Expiration);
	}
	catch(...)
	{
		ret = nullptr;
	}

	return ret;
}

const std::map<std::string, double>& BlackScholesIVM::DefaultParams(void) const
{
	static std::map<std::string, double> defaultParams = {
		{ OptionParams::riskFreeRate_name , GlobalSettings::RiskFreeRate() },
		{ OptionParams::dividend_name , 0 },
	};
	return defaultParams;
}

void BlackScholesIVM::ParseParams(const std::map<std::string, double>& modelParams, std::unique_ptr<ParamsBase>& target)
{
	auto ret = std::make_unique<OptionParams>();

	ret->riskFreeRate = modelParams.at(OptionParams::riskFreeRate_name);
	ret->dividend = modelParams.at(OptionParams::dividend_name);

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
	DayCounter dayCounter = Actual365Fixed();

	Date maturity((Day)maturityDate.Day, (Month)(maturityDate.Month), (Year)(maturityDate.Year));

	Date settlementDate((Day)tradingDate.Day, (Month)(tradingDate.Month), (Year)(tradingDate.Year));

	Time timeToMaturity = dayCounter.yearFraction(settlementDate, maturity);

	Option::Type optionType = contractType == ContractType::CONTRACTTYPE_CALL_OPTION ? Option::Call : Option::Put;

	//auto volatilityPtr = boost::shared_ptr<SimpleQuote>(new SimpleQuote(volatility));
	//Handle<Quote> hQute(volatilityPtr);

	// BlackConstantVol blackVol(settlementDate, TARGET(), hQute, dayCounter);

	const static double accuracy = 1e-3, xMin = 0, xMax = 5;
	const int maxit = 200;

	Volatility guess = (volatility < xMin || volatility > xMax) ? (xMin + xMax) / 2 : volatility;

	Brent solver1D;
	solver1D.setMaxEvaluations(maxit);
	Volatility imv = solver1D.solve([&](const Volatility & sigma) {
		/* volatilityPtr->setValue(sigma);
		Real variance = blackVol.blackVariance(timeToMaturity, strikePrice);
		Real stdDev = std::sqrt(variance);*/
		Real stdDev = std::abs(sigma) * std::sqrt(timeToMaturity);
		BlackCalculator blackCalculator(optionType, strikePrice, underlyingPrice, stdDev);
		return blackCalculator.value() - marketOptionPrice; },
		accuracy, guess, xMin, xMax);


	//Handle<Quote> underlyingH(boost::shared_ptr<Quote>(new SimpleQuote(underlyingPrice)));

	//// bootstrap the yield/dividend/vol curves
	//Handle<YieldTermStructure> flatTermStructure(
	//	boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, riskFreeRate, dayCounter)));
	//Handle<YieldTermStructure> flatDividendTS(
	//	boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, dividendYield, dayCounter)));
	//Handle<BlackVolTermStructure> flatVolTS(
	//	boost::shared_ptr<BlackVolTermStructure>(new BlackConstantVol(settlementDate, TARGET(), volatility, dayCounter)));

	//boost::shared_ptr<StrikedTypePayoff> payoff(
	//	new PlainVanillaPayoff(optionType, strikePrice));

	//// options
	//boost::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(maturity));

	//VanillaOption europeanOption(payoff, europeanExercise);

	//boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(
	//	new BlackScholesMertonProcess(underlyingH, flatDividendTS, flatTermStructure, flatVolTS));

	//Volatility imv = europeanOption.impliedVolatility(marketOptionPrice, bsmProcess, 1e-4, 100, 1e-2, 2);

	return imv;
}