#include "GlobalSettings.h"

double & GlobalSettings::RiskFreeRate()
{
	static double rate = 0.03;
	return rate;
}
