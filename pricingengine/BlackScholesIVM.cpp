/***********************************************************************
 * Module:  BlackScholesIVM.cpp
 * Author:  milk
 * Modified: 2016年5月9日 22:57:32
 * Purpose: Implementation of the class BlackScholesIVM
 ***********************************************************************/

#include "BlackScholesIVM.h"

#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/TradingDeskOptionParams.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractParamDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TemplateDO.h"
#include "../bizutility/GlobalSettings.h"

#include "../message/BizError.h"

#include <ql/quantlib.hpp>

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
	if (!sdo.IVMContracts || sdo.IVMContracts->PricingContracts.empty())
	{
		throw NotFoundException("Cannot find IVM pricing contract for " + sdo.InstrumentID() + '.' + sdo.ExchangeID());
	}

	if (!sdo.IVModel->ParsedParams)
	{
		ParseParams(sdo.IVModel->Params, sdo.IVModel->ParsedParams);
	}

	auto paramObj = (OptionParams*)sdo.IVModel->ParsedParams.get();

	if (!sdo.IVMContracts)
		return nullptr;

	MarketDataDO mDO;
	if (!priceCtx.GetMarketDataMap()->find(sdo.InstrumentID(), mDO) ||
		mDO.Ask().Volume <= 0 && mDO.Bid().Volume <= 0)
		return nullptr;

	MarketDataDO mBaseDO;
	if (!priceCtx.GetMarketDataMap()->find(sdo.IVMContracts->PricingContracts[0].InstrumentID(), mBaseDO))
		return nullptr;

	if (mBaseDO.Ask().Volume <= 0 && mBaseDO.Bid().Volume <= 0)
		return nullptr;

	double base_bidPrice = mBaseDO.Bid().Price;
	double base_askdPrice = mBaseDO.Ask().Price;

	double adjust = sdo.IVMContracts->PricingContracts[0].Adjust;
	double price = (base_bidPrice + base_askdPrice) / 2 + adjust;

	auto ret = std::make_shared<TDataObject<ImpliedVolatility>>();
	try
	{
		if (mDO.Bid().Volume > 0)
			ret->Data.BidVolatility = CaclImpliedVolatility(mDO.Bid().Price, price, sdo.StrikePrice, paramObj->bidVolatility, paramObj->riskFreeRate, paramObj->dividend, sdo.ContractType, sdo.TradingDay, sdo.Expiration);
		else
			ret->Data.BidVolatility = std::nan(nullptr);
	}
	catch (QuantLib::Error& e)
	{
		paramObj->bidVolatility = OptionParams::DEFAULT_VOLATILITY();
		ret->Data.BidVolatility = std::nan(nullptr);
	}

	try
	{
		if (mDO.Ask().Volume > 0)
			ret->Data.AskVolatility = CaclImpliedVolatility(mDO.Ask().Price, price, sdo.StrikePrice, paramObj->askVolatility, paramObj->riskFreeRate, paramObj->dividend, sdo.ContractType, sdo.TradingDay, sdo.Expiration);
		else
			ret->Data.AskVolatility = std::nan(nullptr);
	}
	catch (QuantLib::Error& e)
	{
		paramObj->askVolatility = OptionParams::DEFAULT_VOLATILITY();
		ret->Data.AskVolatility = std::nan(nullptr);
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

double BlackScholesIVM::CaclImpliedVolatility(
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

	Option::Type optionType = contractType == ContractType::CONTRACTTYPE_CALL_OPTION ? Option::Call : Option::Put;

	//auto volatilityPtr = boost::shared_ptr<SimpleQuote>(new SimpleQuote(volatility));
	//Handle<Quote> hQute(volatilityPtr);

	// BlackConstantVol blackVol(settlementDate, TARGET(), hQute, dayCounter);

	const static int maxit = 200;
	const static double xMin = 0;
	const static double default_vol = OptionParams::DEFAULT_VOLATILITY();
	const static double default_vol_2 = default_vol + default_vol;
	double accuracy = 1e-3;
	double xMax = 10;
	if (default_vol != volatility)
	{
		accuracy = 1e-4;
		xMax = 2 * volatility;
		if (xMax < default_vol_2)
			xMax = default_vol_2;
	}

	/*Volatility guess = (volatility < xMin || volatility > xMax) ? (xMin + xMax) / 2 : volatility;

	Brent solver1D;
	solver1D.setMaxEvaluations(maxit);
	Time timeToMaturity = dayCounter.yearFraction(settlementDate, maturity);
	timeToMaturity = std::sqrt(timeToMaturity);
	Volatility imv = solver1D.solve([&](const Volatility & sigma) {
		Real stdDev = std::abs(sigma) * timeToMaturity;
		BlackCalculator blackCalculator(optionType, strikePrice, underlyingPrice, stdDev);
		return blackCalculator.value() - marketOptionPrice; },
		accuracy, guess, xMin, xMax);*/


	Handle<Quote> underlyingH(boost::shared_ptr<Quote>(new SimpleQuote(underlyingPrice)));

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

	Volatility imv = europeanOption.impliedVolatility(marketOptionPrice, bsmProcess, accuracy, maxit, xMin, xMax);

	return imv;
}