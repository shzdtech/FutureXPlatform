#include "BsBinPricingAlgorithm.h"

#include <ql/quantlib.hpp>

const std::string & BsBinPricingAlgorithm::Name(void) const
{
	static const std::string bs("bin");
	return bs;
}

void BsBinPricingAlgorithm::ComputeOptionPrice(
	double underlyingPrice,
	double strikePrice, 
	double volatility, 
	double riskFreeRate, 
	double dividendYield, 
	ContractType contractType, 
	const DateType & tradingDate, 
	const DateType & maturityDate, 
	OptionPricing & option)
{
	using namespace QuantLib;

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

	boost::shared_ptr<PricingEngine> pricingEngine(new BinomialVanillaEngine<JarrowRudd>(bsmProcess, 801));

	// options
	boost::shared_ptr<Exercise> americanExercise(new AmericanExercise(settlementDate, maturity));

	VanillaOption americanOption(payoff, americanExercise);
	americanOption.setPricingEngine(pricingEngine);

	option.Price = americanOption.NPV();
	option.Delta = americanOption.delta();
	// option.Vega = americanOption.vega();
	option.Gamma = americanOption.gamma();
	option.Theta = americanOption.theta();
}
