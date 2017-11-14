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
#include "../CTPServer/CTPTradeProcessor.h"
#include "../message/SessionContainer.h"
#include "../dataobject/StrategyContractDO.h"
#include "../OTCServer/OTCTradeProcessor.h"
#include "../pricingengine/PricingDataContext.h"
#include "../utility/lockfree_set.h"

#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCTradeProcessor : public CTPTradeProcessor, public OTCTradeProcessor
{
public:
	CTPOTCTradeProcessor();
	CTPOTCTradeProcessor(const CTPRawAPI_Ptr& rawAPI);
	~CTPOTCTradeProcessor();

	virtual void OnTraded(const TradeRecordDO_Ptr & tradeDO);

	virtual OrderDO_Ptr CreateOrder(const OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelOrder(const OrderRequestDO& orderReq);
	virtual uint32_t GetSessionId(void);

	virtual void RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr);
	virtual OTCOrderManager& GetOTCOrderManager(void);
	virtual AutoOrderManager& GetAutoOrderManager(void);
	virtual HedgeOrderManager& GetHedgeOrderManager(void);



	std::shared_ptr<CTPOTCTradeProcessor> Shared_This();

protected:


private:



public:

	///CTP API;
	///登录请求响应

	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnErrRtnOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo);

	void OnErrRtnOrderAction(CThostFtdcOrderActionField * pOrderAction, CThostFtdcRspInfoField * pRspInfo);

	void OnRtnOrder(CThostFtdcOrderField *pOrder);

	void OnRtnTrade(CThostFtdcTradeField * pTrade);

	///请求查询投资者持仓响应
	void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};

typedef std::shared_ptr<CTPOTCTradeProcessor> CTPOTCTradeProcessor_Ptr;

#endif