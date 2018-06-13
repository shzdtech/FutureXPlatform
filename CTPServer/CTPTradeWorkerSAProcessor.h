#pragma once
#include "CTPTradeProcessor.h"
#include "../message/UserInfo.h"
#include "../ordermanager/PortfolioPositionContext.h"
#include "../pricingengine/IPricingDataContext.h"
#include <thread>
#include <atomic>
#include "ctpexport.h"

class CTPTradeWorkerProcessor;
class CTP_CLASS_EXPORT CTPTradeWorkerSAProcessor : public CTPTradeProcessor
{
public:
	CTPTradeWorkerSAProcessor(CTPTradeWorkerProcessor* pTradeWorkProc);
	CTPTradeWorkerSAProcessor(const CTPRawAPI_Ptr& rawAPI, CTPTradeWorkerProcessor* pTradeWorkProc);
	~CTPTradeWorkerSAProcessor();

	int Login(const std::string & brokerId, const std::string & investorId, const std::string & password, const std::string & serverName = "");
	bool Logged(void);
	IUserInfo& getSystemUser();

protected:
	CTPTradeWorkerProcessor* _pTradeWorkProc;
	std::atomic_flag _loginFlag;
	int _lastErrorCode = 0;
	std::string _lastError;

public:
	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����¼�����ر�
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	///������������ر�
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField * pSettlementInfoConfirm, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast);

};

typedef std::shared_ptr<CTPTradeWorkerSAProcessor> CTPTradeWorkerSAProcessor_Ptr;
typedef std::weak_ptr<CTPTradeWorkerSAProcessor> CTPTradeWorkerSAProcessor_WkPtr;

