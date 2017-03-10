#pragma once
#include "WingsModelReturnDO.h"

class TradingDeskOptionParams : public ContractKey, public dataobjectbase
{
public:
	TradingDeskOptionParams(const char* exchangeID, const char* instrumentID) :
		ContractKey(exchangeID, instrumentID) {}

	TradingDeskOptionParams(const std::string& exchangeID, const std::string& instrumentID) :
		ContractKey(exchangeID, instrumentID) {}

	OptionPricingDO MarketData;
	WingsModelReturnDO TheoData;
	WingsModelReturnDO_Ptr TheoDataTemp;
};