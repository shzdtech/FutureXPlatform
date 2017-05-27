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
#include "../CTPServer/CTPTradeWorkerSAProcessor.h"
#include "../message/SessionContainer.h"
#include "../dataobject/StrategyContractDO.h"
#include "../OTCServer/OTCTradeProcessor.h"
#include "../pricingengine/PricingDataContext.h"
#include "../utility/lockfree_set.h"

#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCTradeProcessor : public CTPTradeWorkerSAProcessor, public OTCTradeProcessor
{
public:
	CTPOTCTradeProcessor(IServerContext* pServerCtx, const IPricingDataContext_Ptr& pricingDataCtx);
	~CTPOTCTradeProcessor();
	virtual bool Dispose(void);


	virtual void OnTraded(const TradeRecordDO_Ptr& tradeDO);

	virtual OrderDO_Ptr CreateOrder(const OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelOrder(const OrderRequestDO& orderReq);
	virtual uint32_t GetSessionId(void);

	virtual void RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr);

	virtual OTCOrderManager& GetOTCOrderManager(void);
	virtual AutoOrderManager& GetAutoOrderManager(void);
	virtual HedgeOrderManager& GetHedgeOrderManager(void);
	virtual UserOrderContext& GetExchangeOrderContext();

	virtual void TriggerHedgeOrderUpdating(const PortfolioKey & portfolioKey);
	virtual void TriggerAutoOrderUpdating(const StrategyContractDO& strategyDO);

protected:
	OTCOrderManager _otcOrderMgr;
	AutoOrderManager _autoOrderMgr;
	HedgeOrderManager _hedgeOrderMgr;

	lockfree_set<UserContractKey, UserContractKeyHash> _autoOrderQueue;
	lockfree_set<PortfolioKey, PortfolioKeyHash> _hedgeOrderQueue;

	std::future<void> _autoOrderWorker;
	std::future<void> _hedgeOrderWorker;

	void AutoOrderWorker();
	void HedgeOrderWorker();

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