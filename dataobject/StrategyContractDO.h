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

static const std::string PM("pm");
static const std::string IVM("ivm");
static const std::string VM("vm");

class StrategyPricingContract
{
public:
	std::vector<PricingContract> PricingContracts;
};

typedef std::shared_ptr<StrategyPricingContract> StrategyPricingContract_Ptr;

class StrategyContractDO : public UserContractKey, public PortfolioKey, public dataobjectbase
{
public:
	StrategyContractDO(const std::string& exchangeID, const std::string& instrumentID,
		const std::string& userID, const std::string& portfolioID)
		: UserKey(userID), UserContractKey(exchangeID, instrumentID, userID), PortfolioKey(portfolioID, userID){}

	StrategyContractDO(const std::string& exchangeID, const std::string& instrumentID, const std::string& userID)
		: StrategyContractDO(exchangeID, instrumentID, userID, "") {}

	StrategyContractDO(const std::string& exchangeID, const std::string& instrumentID)
		: StrategyContractDO(exchangeID, instrumentID, "", "") {}

	std::string StrategyName;
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
	StrategyPricingContract_Ptr PricingContracts;

	ModelParamsDO_Ptr IVModel;
	StrategyPricingContract_Ptr IVMContracts;

	ModelParamsDO_Ptr VolModel;
	StrategyPricingContract_Ptr VolContracts;

	void DeepCopyPricingContract(const StrategyContractDO& st)
	{
		if (st.PricingContracts)
			PricingContracts = std::make_shared<StrategyPricingContract>(*PricingContracts);

		if (st.IVMContracts)
			IVMContracts = std::make_shared<StrategyPricingContract>(*IVMContracts);

		if (st.VolContracts)
			VolContracts = std::make_shared<StrategyPricingContract>(*VolContracts);
	}

protected:

private:

};

typedef std::pair<std::string, std::string> UserStrategyName;

typedef std::shared_ptr<StrategyContractDO> StrategyContractDO_Ptr;


#endif