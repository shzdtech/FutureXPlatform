/***********************************************************************
 * Module:  BlackScholesPricingAlgorithm.cpp
 * Author:  milk
 * Modified: 2016年5月9日 22:57:32
 * Purpose: Implementation of the class BlackScholesPricingAlgorithm
 ***********************************************************************/

#include "BlackScholesPricingAlgorithm.h"

#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/ValuationRiskDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractParamDO.h"
#include "../dataobject/TypedefDO.h"
#include "../systemsettings/GlobalSettings.h"
#include "../message/BizError.h"


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
	const IPricingDataContext_Ptr& priceCtx_Ptr,
	const param_vector* params)
{
	if (!sdo.PricingContracts || sdo.PricingContracts->PricingContracts.empty())
	{
		throw NotFoundException("Cannot find PM pricing contract for " + sdo.InstrumentID() + '.' + sdo.ExchangeID());
	}

	if (!sdo.PricingModel->ParsedParams)
	{
		ParseParams(sdo.PricingModel->Params, sdo.PricingModel->ParsedParams);
	}

	auto paramObj = params && params->size() > 0 ? (OptionParams*)params->at(0) : (OptionParams*)sdo.PricingModel->ParsedParams.get();

	if (!sdo.PricingContracts)
		return nullptr;

	double askPrice, bidPrice;
	double riskFreeRate = std::max(paramObj->riskFreeRate, 0.0);
	double askVolatility = std::max(paramObj->askVolatility, DBL_EPSILON);
	double bidVolatility = std::max(paramObj->bidVolatility, DBL_EPSILON);
	double midVolatility = std::max(paramObj->midVolatility, DBL_EPSILON);

	Date maturityDate((Day)sdo.Expiration.Day, (Month)sdo.Expiration.Month, (Year)sdo.Expiration.Year);

	Date settlementDate((Day)sdo.TradingDay.Day, (Month)sdo.TradingDay.Month, (Year)sdo.TradingDay.Year);

	if (pInputObject)
	{
		auto pValuationDO = (ValuationRiskDO*)pInputObject;
		auto it = pValuationDO->ValuationPrice.find(sdo.PricingContracts->PricingContracts[0].InstrumentID());
		if (it == pValuationDO->ValuationPrice.end())
			return nullptr;

		bidPrice = askPrice = it->second.Price;

		double deltaVol = it->second.Volatility;
		askVolatility = std::max(askVolatility + deltaVol, DBL_EPSILON);
		bidVolatility = std::max(bidVolatility + deltaVol, DBL_EPSILON);
		midVolatility = std::max(midVolatility + deltaVol, DBL_EPSILON);

		if (pValuationDO->DaysRemain > 0)
			settlementDate = settlementDate + pValuationDO->DaysRemain;
	}
	else
	{
		MarketDataDO mDO;
		if (!priceCtx_Ptr->GetMarketDataMap()->find(sdo.PricingContracts->PricingContracts[0].InstrumentID(), mDO))
			return nullptr;

		/*if (mDO.Ask().Volume <= 0 && mDO.Bid().Volume <= 0)
			return nullptr;*/

		bidPrice = mDO.Bid().Price;
		askPrice = mDO.Ask().Price;

		double adjust = sdo.PricingContracts->PricingContracts[0].Adjust;
		bidPrice += adjust;
		askPrice += adjust;
	}

	if (bidPrice <= 0 || askPrice <= 0)
		return nullptr;

	if (sdo.ContractType == ContractType::CONTRACTTYPE_PUT_OPTION)
	{
		std::swap(bidPrice, askPrice);
	}

	settlementDate = std::min(settlementDate, maturityDate);
	maturityDate++; // Add one day


	auto pricingDO = std::make_shared<OptionPricingDO>(sdo.ExchangeID(), sdo.InstrumentID());

	auto greeks
		= ComputeOptionPrice((bidPrice + askPrice) / 2, sdo.StrikePrice, midVolatility, riskFreeRate, paramObj->dividend, sdo.ContractType, settlementDate, maturityDate);

	pricingDO->Delta = greeks->delta();
	pricingDO->Gamma = greeks->gamma();
	pricingDO->Theta = greeks->theta();
	pricingDO->Vega = greeks->vega();
	pricingDO->Rho = greeks->rho();

	if (pInputObject)
	{
		pricingDO->TBid().Price = pricingDO->TAsk().Price = greeks->NPV();
	}
	else
	{
		auto bidOption
			= ComputeOptionPrice(bidPrice, sdo.StrikePrice, bidVolatility, riskFreeRate, paramObj->dividend, sdo.ContractType, settlementDate, maturityDate);
		double priceBid = std::max(0.0, bidOption->NPV());


		auto askOption
			= ComputeOptionPrice(askPrice, sdo.StrikePrice, askVolatility, riskFreeRate, paramObj->dividend, sdo.ContractType, settlementDate, maturityDate);
		double priceAsk = askOption->NPV();

		if (sdo.NotCross)
		{
			MarketDataDO mdo;
			priceCtx_Ptr->GetMarketDataMap()->find(sdo.InstrumentID(), mdo);
			if (mdo.Ask().Price < DBL_EPSILON)
			{
				mdo.Bid().Price = 1e32;
				mdo.Ask().Price = 0;
			}

			priceBid = std::min(priceBid, mdo.Bid().Price);
			priceAsk = std::max(priceAsk, mdo.Ask().Price);
		}

		auto tickSize = sdo.EffectiveTickSize();
		pricingDO->TBid().Price = std::floor(priceBid / tickSize) * tickSize;
		pricingDO->TAsk().Price = std::ceil(priceAsk / tickSize) * tickSize;
	}

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


std::shared_ptr<VanillaOption> BlackScholesPricingAlgorithm::ComputeOptionPrice(
	double underlyingPrice,
	double strikePrice,
	double volatility,
	double riskFreeRate,
	double dividendYield,
	ContractType contractType,
	const Date& settleDate,
	const Date& maturityDate)
{
	DayCounter dayCounter = Actual365Fixed();

	Option::Type optionType = contractType == ContractType::CONTRACTTYPE_CALL_OPTION ? Option::Call : Option::Put;

	Handle<Quote> underlyingH(boost::shared_ptr<Quote>(new SimpleQuote(underlyingPrice)));

	// bootstrap the yield/dividend/vol curves
	Handle<YieldTermStructure> flatTermStructure(
		boost::shared_ptr<YieldTermStructure>(new FlatForward(settleDate, riskFreeRate, dayCounter)));
	Handle<YieldTermStructure> flatDividendTS(
		boost::shared_ptr<YieldTermStructure>(new FlatForward(settleDate, dividendYield, dayCounter)));
	Handle<BlackVolTermStructure> flatVolTS(
		boost::shared_ptr<BlackVolTermStructure>(new BlackConstantVol(settleDate, TARGET(), volatility, dayCounter)));

	boost::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(optionType, strikePrice));

	boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(new BlackScholesMertonProcess(underlyingH, flatDividendTS, flatTermStructure, flatVolTS));

	boost::shared_ptr<PricingEngine> pricingEngine(new AnalyticEuropeanEngine(bsmProcess));

	// options
	boost::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(maturityDate));

	auto europeanOption = std::make_shared<VanillaOption>(payoff, europeanExercise);
	europeanOption->setPricingEngine(pricingEngine);

	return europeanOption;
}
