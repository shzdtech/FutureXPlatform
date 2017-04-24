/***********************************************************************
 * Module:  OrderDO.h
 * Author:  milk
 * Modified: 2015年10月14日 2:13:16
 * Purpose: Declaration of the class OrderDO
 ***********************************************************************/

#if !defined(__dataobject_OrderDO_h)
#define __dataobject_OrderDO_h

#include "ContractKey.h"
#include "TemplateDO.h"
#include "EnumTypes.h"
#include "PortfolioDO.h"
#include "StrategyContractDO.h"

enum OrderStatusType
{
	UNDEFINED = 0,
	ALL_TRADED = 1,
	PARTIAL_TRADED = 2,
	CANCELED = 3,
	OPEN_REJECTED = 4,
	REJECTED = 5,
	OPENED = 10,
	PARTIAL_TRADING = 11,
	CANCELING = 12,
	CANCEL_REJECTED = 13,
	OPENING = 14
};

enum OrderExecType
{
	LIMIT = 0,
	MARKET = 1,
};

enum OrderTIFType
{
	GFD = 0,
	IOC = 1,
};

class OrderRequestDO : public UserContractKey, public PortfolioKey, public dataobjectbase
{
public:
	OrderRequestDO(const uint64_t orderID, const char* exchangeID, const char* instrumentID,
		const char* userID, const char* portfolioID) : OrderID(orderID), UserKey(userID),
		UserContractKey(exchangeID, instrumentID, userID), PortfolioKey(portfolioID, userID) {}

	OrderRequestDO(const uint64_t orderID, const std::string& exchangeID, const std::string& instrumentID,
		const std::string& userID, const std::string& portfolioID) : OrderID(orderID), UserKey(userID), 
		UserContractKey(exchangeID, instrumentID, userID), PortfolioKey(portfolioID, userID) {}

	OrderRequestDO(const UserContractKey& userContractKey) : 
		UserKey(userContractKey), UserContractKey(userContractKey), PortfolioKey("", userContractKey.UserID()) {}

	OrderRequestDO(const StrategyContractDO& strategy) : UserKey(strategy.UserID()), UserContractKey(strategy), PortfolioKey(strategy) {}

	uint64_t OrderID = 0;
	uint64_t OrderSysID = 0;
	int SessionID = 0;
	double LimitPrice = 0;
	int Volume = 0;
	int ErrorCode = 0;
	DirectionType Direction = DirectionType::SELL;
	OrderOpenCloseType OpenClose = OrderOpenCloseType::OPEN;
	OrderTIFType TIF = OrderTIFType::GFD;
	OrderExecType ExecType = OrderExecType::LIMIT;
	OrderTradingType TradingType = OrderTradingType::TRADINGTYPE_MANUAL;
	OrderConditionFiled ConditionField = OrderConditionFiled::CON_FLD_UNSPECIFIED;
	double ConValueUpper = DBL_MAX;
	double ConValueLower = 0;
};


class OrderDO : public OrderRequestDO
{
public:
	OrderDO(const uint64_t orderID, const char* exchangeID, const char* instrumentID,
		const char* userID, const char* portfolioID = "") : UserKey(userID), 
		OrderRequestDO(orderID, exchangeID, instrumentID, userID, portfolioID) {}

	OrderDO(const uint64_t orderID, const std::string& exchangeID, const std::string& instrumentID,
		const std::string& userID, const std::string& portfolioID = "") :
		UserKey(userID), OrderRequestDO(orderID, exchangeID, instrumentID, userID, portfolioID) {}

	OrderDO(const OrderRequestDO& requestDO) : UserKey(requestDO.UserID()),
		OrderRequestDO(requestDO) {}

	OrderDO(const UserContractKey& userContractKey) : 
		UserKey(userContractKey.UserID()), OrderRequestDO(userContractKey) {}

	OrderDO(const uint64_t orderID) : OrderDO(orderID, "", "", "", ""){}

	OrderStatusType OrderStatus = OrderStatusType::UNDEFINED;
	int VolumeTraded = 0;
	int TradingDay;
	bool Active = false;
	double StopPrice = 0;
	std::string BrokerID;
	std::string InsertDate;
	std::string InsertTime;
	std::string UpdateTime;
	std::string CancelTime;
	std::string Message;

	int VolumeRemain()
	{
		return Volume - VolumeTraded;
	}
protected:

private:

};

typedef std::shared_ptr<OrderRequestDO> OrderRequestDO_Ptr;
typedef std::shared_ptr<OrderDO> OrderDO_Ptr;
typedef std::shared_ptr<VectorDO<OrderDO>> OrderDOVec_Ptr;

#endif