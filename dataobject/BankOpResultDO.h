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
	///ƽ̨��ˮ��
	std::string SerialNum;
	///��������
	int	TradingDay;
	///���׷�������
	std::string	TradeDate;
	///����ʱ��
	std::string	TradeTime;
	///���״���
	std::string	TradeCode;
	///������ˮ��
	std::string	BankSerial;
	///�ڻ���˾�ʺ�����
	BankAccountType	FutureAccType;
	///�ڻ���˾�ʺ�����
	BankAccountType	BankAccType;
	///�ڻ���˾��ˮ��
	std::string	FutureSerial;
	///Ӧ�տͻ�����
	double CustFee;
	///Ӧ���ڻ���˾����
	double BrokerFee;
	///�������
	int	ErrorID;
	///������Ϣ
	std::string ErrorMsg;
};

typedef std::shared_ptr<BankOpResultDO> BankOpResultDO_Ptr;

#endif