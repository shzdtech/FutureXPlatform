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
#include "../CTPServer/CTPTradeWorkerProcessorEx.h"
#include "../message/SessionContainer.h"
#include "../dataobject/StrategyContractDO.h"
#include "../OTCServer/OTCTradeProcessor.h"
#include "../pricingengine/PricingDataContext.h"

#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCTradeProcessor : public CTPTradeWorkerProcessorEx, public OTCTradeProcessor
{
public:
	CTPOTCTradeProcessor(IServerContext* pServerCtx, const IPricingDataContext_Ptr& pricingDataCtx);
	~CTPOTCTradeProcessor();

	virtual void OnTraded(const TradeRecordDO_Ptr& tradeDO);

	virtual OrderDO_Ptr CreateOrder(const OrderRequestDO& orderInfo);
	virtual OrderDO_Ptr CancelOrder(const OrderRequestDO& orderInfo);
	virtual uint32_t GetSessionId(void);

	virtual OTCOrderManager* GetOTCOrderManager(void);
	virtual AutoOrderManager* GetAutoOrderManager(void);
	virtual void RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr);

protected:
	OTCOrderManager _otcOrderMgr;
	AutoOrderManager _autoOrderMgr;

private:



public:

	///CTP API;
	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnErrRtnOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo);

	void OnErrRtnOrderAction(CThostFtdcOrderActionField * pOrderAction, CThostFtdcRspInfoField * pRspInfo);

	void OnRtnOrder(CThostFtdcOrderField *pOrder);

	void OnRtnTrade(CThostFtdcTradeField * pTrade);
};

#endif