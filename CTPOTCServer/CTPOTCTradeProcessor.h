/***********************************************************************
 * Module:  CTPOTCTradeProcessor.h
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Declaration of the class CTPOTCTradeProcessor
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCTradeProcessor_h)
#define __CTPOTC_CTPOTCTradeProcessor_h

#include "../ordermanager/OTCOrderManager.h"
#include "../ordermanager/AutoOrderManager.h"
#include "../ordermanager/HedgeOrderManager.h"
#include "../CTPServer/CTPTradeWorkerProcessor.h"
#include "../message/SessionContainer.h"
#include "../dataobject/StrategyContractDO.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCTradeProcessor : public CTPTradeWorkerProcessor
{
public:
	CTPOTCTradeProcessor(const std::map<std::string, std::string>& configMap, IPricingDataContext* pricingCtx);
	~CTPOTCTradeProcessor();
	void Initialize(void);

	bool OnSessionClosing(void);

	void TriggerOrderUpdating(const StrategyContractDO& strategyDO);

	AutoOrderManager& GetAutoOrderManager(void);
	OTCOrderManager& GetOTCOrderManager(void);

protected:
	OTCOrderManager _otcOrderMgr;

	AutoOrderManager _autoOrderMgr;

	IPricingDataContext* _pricingCtx;
private:



public:

	///CTP API;
	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnRtnOrder(CThostFtdcOrderField *pOrder);
};

#endif