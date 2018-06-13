/***********************************************************************
 * Module:  CTPOTCTradeSAProcessor.h
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Declaration of the class CTPOTCTradeSAProcessor
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCTradeSAProcessor_h)
#define __CTPOTC_CTPOTCTradeSAProcessor_h

#include "CTPOTCTradeWorkerProcessor.h"
#include "../CTPServer/CTPTradeWorkerSAProcessor.h"
#include "../ordermanager/OTCOrderManager.h"
#include "../ordermanager/AutoOrderManager.h"
#include "../ordermanager/HedgeOrderManager.h"
#include "../message/SessionContainer.h"
#include "../dataobject/StrategyContractDO.h"
#include "../OTCServer/OTCTradeProcessor.h"

#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCTradeSAProcessor : public CTPTradeWorkerSAProcessor, public IOrderAPI
{
public:
	CTPOTCTradeSAProcessor(CTPOTCTradeWorkerProcessor* pWorkerProc);
	CTPOTCTradeSAProcessor(const CTPRawAPI_Ptr& rawAPI, CTPOTCTradeWorkerProcessor* pWorkerProc);
	~CTPOTCTradeSAProcessor();

	virtual OrderDO_Ptr CreateOrder(const OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelOrder(const OrderRequestDO& orderReq);
	virtual uint32_t GetSessionId(void);

	std::shared_ptr<CTPOTCTradeSAProcessor> Shared_This();

protected:
	CTPOTCTradeWorkerProcessor* getWorkerProcessor(void);

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

	void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};

typedef std::shared_ptr<CTPOTCTradeSAProcessor> CTPOTCTradeSAProcessor_Ptr;

#endif