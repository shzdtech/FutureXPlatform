#if !defined(__dataobject_UserAccountRegisterDO_h)
#define __dataobject_UserAccountRegisterDO_h

#include <string>
#include "dataobjectbase.h"
#include "EnumTypes.h"

class UserAccountRegisterDO : public dataobjectbase
{
public:
	///银行编码
	std::string	BankID;
	///银行分支机构编码
	std::string	BankBranchID;
	///银行帐号
	std::string	BankAccount;
	///期货公司编码
	std::string	BrokerID;
	///期货公司分支机构编码
	std::string	BrokerBranchID;
	///投资者帐号
	std::string	AccountID;
	///证件号码
	std::string	IdentifiedCardNo;
	///客户姓名
	std::string	CustomerName;
	///币种代码
	std::string	CurrencyID;
	///客户类型
	CustomerType CustType;
	///银行帐号类型
	BankAccountType	BankAccType;
	///银行可用金额
	double BankUseAmount;
	///银行可取金额
	double BankFetchAmount;
};

typedef std::shared_ptr<UserAccountRegisterDO> UserAccountRegisterDO_Ptr;

#endif