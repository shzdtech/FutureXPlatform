/***********************************************************************
 * Module:  MarketDataDO.h
 * Author:  milk
 * Modified: 2015年7月10日 23:28:31
 * Purpose: Declaration of the class MarketDataDO
 ***********************************************************************/

#if !defined(__dataobject_MarketDataDO_h)
#define __dataobject_MarketDataDO_h

#include "PricingDO.h"

class MarketDataDO : public PricingDO
{
public:
	MarketDataDO(const std::string& exchangeID, const std::string& instrumentID)
		: PricingDO(exchangeID, instrumentID)	{}

	int BidVolume = 0;
	int AskVolume = 0;

	int Volume = 0;
	double LastPrice = 0;
	double AveragePrice = 0;
	double Turnover = 0;
	double HighestPrice = 0;
	double LowestPrice = 0;

	double PreClosePrice = 0;
	double OpenPrice = 0;
	double UpperLimitPrice = 0;
	double LowerLimitPrice = 0;
	double PreSettlementPrice = 0;
	double SettlementPrice = 0;
	double PreOpenInterest = 0;
	double OpenInterest = 0;

protected:

private:

};

#endif