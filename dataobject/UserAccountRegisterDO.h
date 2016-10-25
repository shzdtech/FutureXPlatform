#if !defined(__dataobject_UserAccountRegisterDO_h)
#define __dataobject_UserAccountRegisterDO_h

#include <string>
#include "dataobjectbase.h"
#include "EnumTypes.h"

class UserAccountRegisterDO : public dataobjectbase
{
public:
	///���б���
	std::string	BankID;
	///���з�֧��������
	std::string	BankBranchID;
	///�����ʺ�
	std::string	BankAccount;
	///�ڻ���˾����
	std::string	BrokerID;
	///�ڻ���˾��֧��������
	std::string	BrokerBranchID;
	///Ͷ�����ʺ�
	std::string	AccountID;
	///֤������
	std::string	IdentifiedCardNo;
	///�ͻ�����
	std::string	CustomerName;
	///���ִ���
	std::string	CurrencyID;
	///�ͻ�����
	CustomerType CustType;
	///�����ʺ�����
	BankAccountType	BankAccType;
	///���п��ý��
	double BankUseAmount;
	///���п�ȡ���
	double BankFetchAmount;
};

typedef std::shared_ptr<UserAccountRegisterDO> UserAccountRegisterDO_Ptr;

#endif