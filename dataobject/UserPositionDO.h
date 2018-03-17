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
	double OpenCost = 0;
	double CloseProfit = 0;
	double UseMargin = 0;
	double LastPrice;
	double SettlementPrice = 0;
	double PreSettlementPrice = 0;
	int Multiplier = 1;

	PositionDateFlagType PositionDateFlag = PSD_TODAY_HISTORY;

	double TdProfit() const {
		double deltaPrice = LastPrice - AvgOpenPrice();
		return TdPosition * (Direction == PositionDirectionType::PD_LONG ? deltaPrice : -deltaPrice) * Multiplier;
	}
	double YdProfit() const {
		double deltaPrice = LastPrice - PreSettlementPrice;
		return LastPosition() * (Direction == PositionDirectionType::PD_LONG ? deltaPrice : -deltaPrice) * Multiplier;
	}
	double AvgPrice() const { return (TdPosition * AvgOpenPrice() + LastPosition() * PreSettlementPrice) / Position(); }

	double AvgCost() const { return TdPosition ? TdCost / TdPosition : 0;  }
	double AvgOpenPrice() const { return OpenVolume ? OpenAmount / OpenVolume : 0; }
	int Position() const { return YdInitPosition + YdPosition + TdPosition; }
	int LastPosition() const { return YdInitPosition + YdPosition; }
	double Cost() const { return YdCost + TdCost; }
	double Profit() const { return YdProfit() + TdProfit(); }

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

class PositionPnLDO : public UserContractKey, public PortfolioKey, public dataobjectbase
{
public:
	PositionPnLDO(const std::string& exchangeID, const std::string& instrumentID, const std::string& portfolioID, const std::string& userID)
		: UserKey(userID), UserContractKey(exchangeID, instrumentID, userID), PortfolioKey(portfolioID, userID) {}
	PositionPnLDO(const std::string& exchangeID, const std::string& instrumentID)
		: PositionPnLDO(exchangeID, instrumentID, "", "") {}
	PositionPnLDO() = default;

	int YdBuyVolume = 0;
	int TdBuyVolume = 0;
	double TdBuyAmount = 0;

	int YdSellVolume = 0;
	int TdSellVolume = 0;
	double TdSellAmount = 0;

	int Multiplier = 1;
	double LastPrice;
	double PreSettlementPrice = 0;

	double TdBuyProfit() const {
		double deltaPrice = LastPrice - BuyAvgPrice();
		return TdSellVolume * deltaPrice * Multiplier;
	}

	double TdSellProfit() const {
		double deltaPrice = SellAvgPrice() - LastPrice;
		return TdSellVolume * deltaPrice * Multiplier;
	}

	double YdBuyProfit() const {
		double deltaPrice = LastPrice - PreSettlementPrice;
		return YdSellVolume * deltaPrice * Multiplier;
	}

	double YdSellProfit() const {
		double deltaPrice = PreSettlementPrice - LastPrice;
		return YdSellVolume * deltaPrice * Multiplier;
	}

	double BuyProfit() const { return YdBuyProfit() + TdBuyProfit(); }
	double SellProfit() const { return YdSellProfit() + TdSellProfit(); }
	double NetProfit() const { return BuyProfit() + SellProfit(); }

	int BuyPosition() const { return YdBuyVolume + TdBuyVolume; }
	int SellPosition() const { return YdSellVolume + TdSellVolume; }
	int NetPosition() const { return BuyPosition() - SellPosition(); };

	double BuyAvgPrice() const { return BuyPosition() > 0 ? ((TdBuyAmount / Multiplier + YdBuyVolume * PreSettlementPrice) / BuyPosition()) : 0; }
	double SellAvgPrice() const { return SellPosition() > 0 ? ((TdSellAmount / Multiplier + YdSellVolume * PreSettlementPrice) / SellPosition()) : 0; }
};

typedef std::shared_ptr<UserPositionDO> UserPositionDO_Ptr;
typedef std::shared_ptr<UserPositionExDO> UserPositionExDO_Ptr;

typedef std::shared_ptr<PositionPnLDO> PositionPnLDO_Ptr;
#endif