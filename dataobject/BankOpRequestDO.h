#if !defined(__dataobject_BankOpRequestDO_h)
#define __dataobject_BankOpRequestDO_h

#include <string>
#include "dataobjectbase.h"
#include "EnumTypes.h"

class BankOpRequestDO : public dataobjectbase
{
public:
	///银行编码
	std::string	BankID;
	///银行分支机构编码
	std::string	BankBranchID;
	///银行密码
	std::string BankPassword;
	///银行帐号
	std::string	BankAccount;
	///期货公司编码
	std::string	BrokerID;
	///期商分支机构代码
	std::string	BrokerBranchID;
	///投资者帐号
	std::string	AccountID;
	///投资者密码
	std::string Password; 
	///币种代码
	std::string	CurrencyID;

	double TradeAmount;  
};

#endif