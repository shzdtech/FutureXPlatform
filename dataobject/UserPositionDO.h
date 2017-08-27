/***********************************************************************
 * Module:  UserPositionDO.h
 * Author:  milk
 * Modified: 2015年7月12日 15:10:23
 * Purpose: Declaration of the class UserPositionDO
 ***********************************************************************/

#if !defined(__dataobject_UserPositionDO_h)
#define __dataobject_UserPositionDO_h

#include "ContractKey.h"
#include "PortfolioDO.h"
#include "dataobjectbase.h"
#include "EnumTypes.h"

class UserPositionDO : public UserContractKey, public PortfolioKey, public dataobjectbase
{
public:
	UserPositionDO(const std::string& exchangeID, const std::string& instrumentID, const std::string& portfolioID, const std::string& userID)
		: UserKey(userID), UserContractKey(exchangeID, instrumentID, userID), PortfolioKey(portfolioID, userID) {}
	UserPositionDO(const std::string& exchangeID, const std::string& instrumentID)
		: UserPositionDO(exchangeID, instrumentID, "", "") {}
	UserPositionDO() = default;

	PositionDirectionType Direction = PositionDirectionType::PD_NET;
	int TradingDay;

	int YdPosition = 0;
	int TdPosition = 0;
	int YdInitPosition = 0;
	int OpenVolume = 0;
	int CloseVolume = 0;
	HedgeType HedgeFlag = HedgeType::HEDGETYPE_SPECULATION;

	double OpenAmount = 0;
	double CloseAmount = 0;
	double TdCost = 0;
	double YdCost = 0;
	double TdProfit = 0;
	double YdProfit = 0;
	double OpenCost = 0;
	double CloseProfit = 0;
	double UseMargin = 0;
	
	PositionDateFlagType PositionDateFlag = PSD_TODAY_HISTORY;

	int Position() const { return YdInitPosition + YdPosition + TdPosition; }
	int LastPosition() const { return YdInitPosition + YdPosition; }
	double Cost() const { return YdCost + TdCost; }
	double Profit() const { return YdProfit + TdProfit; }

protected:

private:

};

class UserPositionExDO : public UserPositionDO
{
public:
	UserPositionExDO(const std::string& exchangeID, const std::string& instrumentID, const std::string& portfolioID, const std::string& userID)
		: UserKey(userID), UserPositionDO(exchangeID, instrumentID, portfolioID, userID) {}
	UserPositionExDO(const std::string& exchangeID, const std::string& instrumentID)
		: UserPositionDO(exchangeID, instrumentID) {}
	UserPositionExDO() = default;

	int SettlementID = 0;
	int LongFrozenVolume = 0;
	int ShortFrozenVolume = 0;
	double LongFrozenAmount = 0;
	double ShortFrozenAmount = 0;
	double SettlementPrice = 0;
	double PreSettlementPrice = 0;
	double ExchangeMargin = 0;
	double CashIn = 0;
	double CombPosition = 0;
	double CombLongFrozen = 0;
	double CombShortFrozen = 0;
	double Commission = 0;
	//double CloseProfitByDate = 0;
	//double CloseProfitByTrade = 0;
	double FrozenMargin = 0;
	double FrozenCash = 0;
	double FrozenCommission = 0;
	//double MarginRateByMoney = 0;
	//double MarginRateByVolume = 0;
	double PreMargin = 0;
};

typedef std::shared_ptr<UserPositionDO> UserPositionDO_Ptr;
typedef std::shared_ptr<UserPositionExDO> UserPositionExDO_Ptr;

#endif