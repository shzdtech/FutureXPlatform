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
#include "../CTPServer/CTPTradeWorkerProcessor.h"
#include "../message/SessionContainer.h"
#include "../dataobject/StrategyContractDO.h"
#include "../OTCServer/OTCTradeWorkerProcessor.h"
#include "../pricingengine/PricingDataContext.h"
#include "../utility/lockfree_set.h"
#include "CTPOTCTradeProcessor.h"
#include "CTPOTCTradeWorkerProcessorBase.h"

#include "ctpotc_export.h"

class CTPOTCWorkerProcessor;

class CTP_OTC_CLASS_EXPORT CTPOTCTradeWorkerProcessor : public CTPOTCTradeWorkerProcessorBase, public CTPTradeWorkerProcessor
{
public:
	CTPOTCTradeWorkerProcessor(IServerContext* pServerCtx, const IPricingDataContext_Ptr& pricingDataCtx, const IUserPositionContext_Ptr& positionCtx);
	~CTPOTCTradeWorkerProcessor();

	virtual CTPTradeWorkerSAProcessor_Ptr CreateSAProcessor();

	virtual void OnTraded(const TradeRecordDO_Ptr& tradeDO);

	virtual OrderDO_Ptr CreateOrder(const OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelOrder(const OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelAutoOrder(const UserContractKey & userContractKey);
	virtual OrderDO_Ptr CancelHedgeOrder(const PortfolioKey & portfolioKey);
	virtual uint32_t GetSessionId(void);

	virtual SessionContainer_Ptr<std::string>& GetUserSessionContainer(void);
	virtual IUserPositionContext_Ptr& GetUserPositionContext();
	virtual UserTradeContext& GetUserTradeContext();
	virtual 
	virtual UserOrderContext& GetExchangeOrderContext(void);

protected:
	virtual bool& Closing(void);

public:

	///CTP API;
	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnErrRtnOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo);

	void OnErrRtnOrderAction(CThostFtdcOrderActionField * pOrderAction, CThostFtdcRspInfoField * pRspInfo);

	void OnRtnOrder(CThostFtdcOrderField *pOrder);

	void OnRtnTrade(CThostFtdcTradeField * pTrade);
};

#endif