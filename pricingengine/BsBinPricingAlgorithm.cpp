#include "BsBinPricingAlgorithm.h"

using namespace QuantLib;

const std::string & BsBinPricingAlgorithm::Name(void) const
{
	static const std::string bs("bin");
	return bs;
}

std::shared_ptr<VanillaOption> BsBinPricingAlgorithm::ComputeOptionPrice(
	double underlyingPrice,
	double strikePrice, 
	double volatility, 
	double riskFreeRate, 
	double dividendYield, 
	ContractType contractType, 
	const Date& settleDate,
	const Date& maturityDate)
{
	using namespace QuantLib;

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

	boost::shared_ptr<PricingEngine> pricingEngine(new BinomialVanillaEngine<JarrowRudd>(bsmProcess, 801));

	// options
	boost::shared_ptr<Exercise> americanExercise(new AmericanExercise(settleDate, maturityDate));

	auto americanOption = std::make_shared<VanillaOption>(payoff, americanExercise);
	americanOption->setPricingEngine(pricingEngine);

	return americanOption;
}
