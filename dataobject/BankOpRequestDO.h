#if !defined(__dataobject_BankOpRequestDO_h)
#define __dataobject_BankOpRequestDO_h

#include <string>
#include "dataobjectbase.h"
#include "EnumTypes.h"

class BankOpRequestDO : public dataobjectbase
{
public:
	///���б���
	std::string	BankID;
	///���з�֧��������
	std::string	BankBranchID;
	///��������
	std::string BankPassword;
	///�����ʺ�
	std::string	BankAccount;
	///�ڻ���˾����
	std::string	BrokerID;
	///���̷�֧��������
	std::string	BrokerBranchID;
	///Ͷ�����ʺ�
	std::string	AccountID;
	///Ͷ��������
	std::string Password; 
	///���ִ���
	std::string	CurrencyID;

	double TradeAmount;  
};

#endif