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

enum OrderStatus
{
	UNDEFINED = 0,
	ALL_TRADED = 1,
	PARTIAL_TRADED = 2,
	CANCELED = 3,
	OPEN_REJECTED = 4,
	REJECTED = 5,
	OPENNING = 10,
	PARTIAL_TRADING = 11,
	CANCELING = 12,
	CANCEL_REJECTED = 13
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

class OrderBaseDO : public dataobjectbase
{
public:
	OrderBaseDO(const uint64_t orderID) : OrderID(orderID){}
	uint64_t OrderID = 0;
	uint64_t OrderSysID = 0;
	DirectionType Direction = DirectionType::SELL;
	double LimitPrice = 0;
	int Volume = 0;
	bool Active = false;
	OrderStatus OrderStatus = OrderStatus::UNDEFINED;
	int ErrorCode = 0;
};


class OrderDO : public UserContractKey, public OrderBaseDO
{
public:
	OrderDO(const uint64_t orderID, const char* exchangeID, const char* instrumentID,
		const char* userID) : OrderBaseDO(orderID),
		UserContractKey(exchangeID, instrumentID, userID){}

	OrderDO(const uint64_t orderID, const std::string& exchangeID, const std::string& instrumentID,
		const std::string& userID) : OrderBaseDO(orderID), 
		UserContractKey(exchangeID, instrumentID, userID){}

	OrderDO(const UserContractKey& userContractKey) : OrderBaseDO(0), UserContractKey(userContractKey){}

	OrderDO(const uint64_t orderID) : OrderDO(orderID, "", "", ""){}

	OrderOpenCloseType OpenClose = OrderOpenCloseType::OPEN;
	OrderTIFType TIF = OrderTIFType::GFD;
	TradingType TradingType = TradingType::TRADINGTYPE_MANUAL;
	OrderExecType ExecType = OrderExecType::LIMIT;
	int VolumeTraded = 0;
	int VolumeRemain = 0;
	int TradingDay = 0;
	double StopPrice = 0;
	std::string InsertTime;
	std::string UpdateTime;
	std::string CancelTime;
	std::string Message;

protected:

private:

};

typedef std::shared_ptr<OrderDO> OrderDO_Ptr;
typedef std::shared_ptr<VectorDO<OrderDO>> OrderDOVec_Ptr;

#endif