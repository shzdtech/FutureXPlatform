#include "BsBinIVM.h"

#include <ql/quantlib.hpp>

using namespace QuantLib;

const std::string& BsBinIVM::Name(void) const
{
	static const std::string bs("binivm");
	return bs;
}

double BsBinIVM::CaclImpliedVolatility(double marketOptionPrice, double underlyingPrice, double strikePrice, double volatility, double riskFreeRate, double dividendYield, ContractType contractType, const DateType & tradingDate, const DateType & maturityDate)
{
	DayCounter dayCounter = Actual365Fixed();

	Date maturity((Day)maturityDate.Day, (Month)(maturityDate.Month), (Year)(maturityDate.Year));

	Date settlementDate((Day)tradingDate.Day, (Month)(tradingDate.Month), (Year)(tradingDate.Year));

	Option::Type optionType = contractType == ContractType::CONTRACTTYPE_CALL_OPTION ? Option::Call : Option::Put;

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

	Volatility imv = americanOption.impliedVolatility(marketOptionPrice, bsmProcess, accuracy, maxit, xMin, xMax);

	return imv;
}
