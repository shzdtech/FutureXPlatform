/***********************************************************************
 * Module:  StrategyContractDO.h
 * Author:  milk
 * Modified: 2015年8月8日 22:13:25
 * Purpose: Declaration of the class StrategyContractDO
 ***********************************************************************/

#if !defined(__dataobject_StrategyContractDO_h)
#define __dataobject_StrategyContractDO_h

#include "AbstractDataObj.h"
#include "ContractParam.h"
#include "ContractKey.h"
#include "PortfolioDO.h"

class StrategyContractDO : public UserContractKey, public PortfolioKey, public AbstractDataObj
{
public:
	StrategyContractDO(const std::string& exchangeID, const std::string& instrumentID,
		const std::string& userID, const std::string& portfolioID)
		: UserContractKey(exchangeID, instrumentID, userID), PortfolioKey(portfolioID, userID){}

	StrategyContractDO(const std::string& exchangeID, const std::string& instrumentID)
		: StrategyContractDO(exchangeID, instrumentID, "", "") {}

	std::string Strategy;
	std::string Description;
	std::string Underlying;
	std::string Algorithm;
	int Quantity = 1;
	int Depth = 2;
	double Spread = 0;
	double Offset = 0;
	double TickSize = 1;
	double Multiplier = 1;
	double Strike = 0;
	double RiskFreeRate = 0;
	double Volatility;
	bool Trading = false;
	bool Enabled = false;

	std::tm SettlementDate;
	std::tm Muturity;

	std::shared_ptr<std::map<std::string, double>> ParamMap;
	std::shared_ptr<std::vector<ContractParam>> BaseContracts;

protected:

private:

};

typedef std::shared_ptr<StrategyContractDO> StrategyContractDO_Ptr;


#endif