#pragma once
#include "PricingDO.h"

class TradingDeskOptionParams : public ContractKey, public dataobjectbase
{
public:
	TradingDeskOptionParams(const char* exchangeID, const char* instrumentID) :
		ContractKey(exchangeID, instrumentID), MarketData(exchangeID, instrumentID), TheoData(exchangeID, instrumentID){}

	TradingDeskOptionParams(const std::string& exchangeID, const std::string& instrumentID) :
		ContractKey(exchangeID, instrumentID), MarketData(exchangeID, instrumentID), TheoData(exchangeID, instrumentID) {}

	OptionPricingDO MarketData;
	OptionPricingDO TheoData;
	
};