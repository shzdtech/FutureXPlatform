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
#include "ModelParamsDO.h"
#include "EnumTypes.h"
#include "DateType.h"
#include <ctime>

class StrategyContractDO : public UserContractKey, public PortfolioKey, public dataobjectbase
{
public:
	StrategyContractDO(const std::string& exchangeID, const std::string& instrumentID,
		const std::string& userID, const std::string& portfolioID)
		: UserKey(userID), UserContractKey(exchangeID, instrumentID, userID), PortfolioKey(portfolioID, userID){}

	StrategyContractDO(const std::string& exchangeID, const std::string& instrumentID)
		: StrategyContractDO(exchangeID, instrumentID, "", "") {}

	std::string StrategyName;
	std::string Description;
	std::string Underlying;
	DateType Expiration;
	DateType TradingDay;
	ProductType ProductType;
	ContractType ContractType;
	int BidQT = 1;
	int AskQT = 1;
	int Depth = 2;
	bool Hedging = false;
	bool BidEnabled = true;
	bool AskEnabled = true;

	double TickSize = 1;
	double Multiplier = 1;
	double StrikePrice;

	ModelParamsDO_Ptr PricingModel;
	ModelParamsDO_Ptr IVModel;
	ModelParamsDO_Ptr VolModel;

	std::vector<PricingContract> PricingContracts;

protected:

private:

};

typedef std::pair<std::string, std::string> UserStrategyName;

typedef std::shared_ptr<StrategyContractDO> StrategyContractDO_Ptr;


#endif