/***********************************************************************
 * Module:  StrategyContractDO.h
 * Author:  milk
 * Modified: 2015年8月8日 22:13:25
 * Purpose: Declaration of the class StrategyContractDO
 ***********************************************************************/

#if !defined(__dataobject_StrategyContractDO_h)
#define __dataobject_StrategyContractDO_h

#include "dataobjectbase.h"
#include "PricingContract.h"
#include "ContractKey.h"
#include "PortfolioDO.h"
#include <ctime>

class StrategyContractDO : public UserContractKey, public PortfolioKey, public dataobjectbase
{
public:
	StrategyContractDO(const std::string& exchangeID, const std::string& instrumentID,
		const std::string& userID, const std::string& portfolioID)
		: UserKey(userID), UserContractKey(exchangeID, instrumentID, userID), PortfolioKey(portfolioID, userID){}

	StrategyContractDO(const std::string& exchangeID, const std::string& instrumentID)
		: StrategyContractDO(exchangeID, instrumentID, "", "") {}

	std::string Strategy;
	std::string Description;
	std::string Underlying;
	std::string Algorithm;
	std::tm Expiration;
	int Quantity = 1;
	int Depth = 2;
	double Volatility = 0.1;
	double TickSize = 1;
	double Multiplier = 1;
	bool Trading = false;
	bool Enabled = false;

	std::map<std::string, double> Params;
	std::shared_ptr<std::vector<PricingContract>> PricingContracts;

protected:

private:

};

typedef std::shared_ptr<StrategyContractDO> StrategyContractDO_Ptr;


#endif