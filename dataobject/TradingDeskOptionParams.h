#pragma once
#include "WingsModelReturnDO.h"

class ImpliedVolatility
{
public:
	double BidVolatility = 0;
	double AskVolatility = 0;
};

typedef std::shared_ptr<ImpliedVolatility> ImpliedVolatility_Ptr;

class TradingDeskOptionParams : public ContractKey, public dataobjectbase
{
public:
	TradingDeskOptionParams(const char* exchangeID, const char* instrumentID) :
		ContractKey(exchangeID, instrumentID) {}

	TradingDeskOptionParams(const std::string& exchangeID, const std::string& instrumentID) :
		ContractKey(exchangeID, instrumentID) {}

	PricingDO_Ptr MarketData; 
	ImpliedVolatility_Ptr ImpliedVol;
	WingsModelReturnDO_Ptr TheoData;
	WingsModelReturnDO_Ptr TheoDataTemp;
};