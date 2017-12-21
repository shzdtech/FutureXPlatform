/***********************************************************************
 * Module:  CTPOTCTradeWorkerProcessor.h
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Declaration of the class CTPOTCTradeWorkerProcessor
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCTradeWorkerProcessor_h)
#define __CTPOTC_CTPOTCTradeWorkerProcessor_h

#include "../ordermanager/OTCOrderManager.h"
#include "../ordermanager/AutoOrderManager.h"
#include "../ordermanager/HedgeOrderManager.h"
#include "../CTPServer/CTPTradeWorkerSAProcessor.h"
#include "../message/SessionContainer.h"
#include "../dataobject/StrategyContractDO.h"
#include "../OTCServer/OTCTradeWorkerProcessor.h"
#include "../pricingengine/PricingDataContext.h"
#include "../utility/lockfree_set.h"
#include "CTPOTCTradeProcessor.h"

#include "ctpotc_export.h"

class CTPOTCWorkerProcessor;

class CTP_OTC_CLASS_EXPORT CTPOTCTradeWorkerProcessor : public CTPTradeWorkerSAProcessor, public OTCTradeWorkerProcessor
{
public:
	CTPOTCTradeWorkerProcessor(IServerContext* pServerCtx, const IPricingDataContext_Ptr& pricingDataCtx, const IUserPositionContext_Ptr& positionCtx);
	~CTPOTCTradeWorkerProcessor();
	virtual bool Dispose(void);


	virtual void OnTraded(const TradeRecordDO_Ptr& tradeDO);

	virtual OrderDO_Ptr CreateOrder(const OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelOrder(const OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelAutoOrder(const UserContractKey & userContractKey);
	virtual OrderDO_Ptr CancelHedgeOrder(const PortfolioKey & portfolioKey);
	virtual uint32_t GetSessionId(void);


	virtual OTCOrderManager& GetOTCOrderManager(void);
	virtual AutoOrderManager& GetAutoOrderManager(void);
	virtual HedgeOrderManager& GetHedgeOrderManager(void);
	virtual UserOrderContext& GetExchangeOrderContext();

	virtual void OnNewManualTrade(const TradeRecordDO & tradeDO);

	virtual void TriggerHedgeOrderUpdating(const PortfolioKey & portfolioKey, const OTCTradeProcessor_Ptr& processor_ptr);
	virtual void TriggerAutoOrderUpdating(const UserContractKey& strategyKey, const OTCTradeProcessor_Ptr& processor_ptr);

	std::shared_ptr<CTPOTCWorkerProcessor> GetMDWorkerProcessor();
	void SetMDWorkerProcessor(const std::shared_ptr<CTPOTCWorkerProcessor>& workerProcessor);

protected:
	std::shared_ptr<CTPOTCWorkerProcessor> _mdWorkerProcessor;

	OTCOrderManager _otcOrderMgr;
	AutoOrderManager _autoOrderMgr;
	HedgeOrderManager _hedgeOrderMgr;

	lockfree_set<std::pair<UserContractKey, OTCTradeProcessor_Ptr>, pairhash_first<UserContractKey, OTCTradeProcessor_Ptr, UserContractKeyHash>> _autoOrderQueue;
	lockfree_set<std::pair<PortfolioKey, OTCTradeProcessor_Ptr>, pairhash_first<PortfolioKey, OTCTradeProcessor_Ptr, PortfolioKeyHash>> _hedgeOrderQueue;
	lockfree_set<std::pair<PortfolioKey, OTCTradeProcessor_Ptr>, pairhash_first<PortfolioKey, OTCTradeProcessor_Ptr, PortfolioKeyHash>> _waitingHedgeQueue;

	std::future<void> _autoOrderWorker;
	std::future<void> _hedgeOrderWorker;

	void AutoOrderWorker();
	void HedgeOrderWorker();


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
	///请求查询投资者持仓响应
	void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
 };

#endif