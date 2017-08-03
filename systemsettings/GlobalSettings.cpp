#include "GlobalSettings.h"

double & GlobalSettings::RiskFreeRate()
{
	static double rate = 0.03;
	return rate;
}

double & GlobalSettings::VolatilityEps()
{
	static double eps = 1e-9;
	return eps;
}
