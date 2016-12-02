#if !defined(__dataobject_BankOpResultDO_h)
#define __dataobject_BankOpResultDO_h

#include <string>
#include <memory>
#include "BankOpRequestDO.h"
#include "dataobjectbase.h"
#include "EnumTypes.h"

class BankOpResultDO : public BankOpRequestDO
{
public:
	///平台流水号
	std::string SerialNum;
	///交易日期
	int	TradingDay;
	///交易发起方日期
	std::string	TradeDate;
	///交易时间
	std::string	TradeTime;
	///交易代码
	std::string	TradeCode;
	///银行流水号
	std::string	BankSerial;
	///期货公司帐号类型
	BankAccountType	FutureAccType;
	///期货公司帐号类型
	BankAccountType	BankAccType;
	///期货公司流水号
	std::string	FutureSerial;
	///应收客户费用
	double CustFee;
	///应收期货公司费用
	double BrokerFee;
	///错误代码
	int	ErrorID;
	///错误信息
	std::string ErrorMsg;
};

typedef std::shared_ptr<BankOpResultDO> BankOpResultDO_Ptr;

#endif