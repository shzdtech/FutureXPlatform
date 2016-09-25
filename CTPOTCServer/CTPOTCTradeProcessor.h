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
#include "../OTCServer/OTCTradeProcessor.h"
#include "../pricingengine/PricingDataContext.h"

#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCTradeProcessor : public CTPTradeWorkerProcessor, public OTCTradeProcessor
{
public:
	CTPOTCTradeProcessor(IServerContext* pServerCtx, const IPricingDataContext_Ptr& pricingDataCtx);
	~CTPOTCTradeProcessor();

	virtual OrderDOVec_Ptr TriggerHedgeOrderUpdating(const StrategyContractDO& strategyDO);
	virtual OrderDOVec_Ptr TriggerOTCOrderUpdating(const StrategyContractDO& strategyDO);

	virtual OrderDO_Ptr CreateOrder(OrderRequestDO& orderInfo);
	virtual OrderDO_Ptr CancelOrder(OrderRequestDO& orderInfo);

	virtual OTCOrderManager* GetOTCOrderManager(void);
	virtual AutoOrderManager* GetAutoOrderManager(void);
	virtual void RegisterLoggedSession(IMessageSession * pMessageSession);

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

	void OnRtnOrder(CThostFtdcOrderField *pOrder);

	void OnRtnTrade(CThostFtdcTradeField * pTrade);
};

#endif