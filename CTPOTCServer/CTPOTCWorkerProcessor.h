/***********************************************************************
 * Module:  CTPOTCWorkerProcessor.h
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Declaration of the class CTPOTCWorkerProcessor
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCWorkerProcessor_h)
#define __CTPOTC_CTPOTCWorkerProcessor_h

#include <set>
#include <list>
#include "CTPOTCTradeProcessor.h"
#include "../ordermanager/OTCOrderManager.h"
#include "../CTPServer/CTPMarketDataProcessor.h"
#include "../dataobject/TypedefDO.h"

#include "../message/SessionContainer.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCWorkerProcessor : public CTPMarketDataProcessor
{
public:
   CTPOTCWorkerProcessor(const std::map<std::string, std::string>& configMap, IPricingDataContext* pricingCtx);
   ~CTPOTCWorkerProcessor();
   bool OnSessionClosing(void);
   void setSession(IMessageSession_WkPtr msgSession_wk_ptr);
   void Initialize(void);

   int LoginSystemUserIfNeed(void);
  
   int RefreshStrategy(const StrategyContractDO& strategyDO);
   void AddContractToMonitor(const ContractKey& contractId);
  
   void TriggerPricing(const StrategyContractDO& strategyDO);
   void TriggerUpdating(const MarketDataDO& mdDO);
   void TriggerOTCOrderUpdating(const StrategyContractDO& strategyDO);
   void TriggerOrderUpdating(const StrategyContractDO& strategyDO);
   
   void RegisterPricingListener(const ContractKey& contractId,
	   IMessageSession* pMsgSession);
   void UnregisterPricingListener(const ContractKey& contractId,
	   IMessageSession* pMsgSession);

   void RegisterLoggedSession(IMessageSession* pMessageSession);

   void CancelAutoOrder(const UserContractKey& userContractKey);
   int OTCNewOrder(OrderDO& orderReq);
   int OTCCancelOrder(OrderDO& orderReq);

protected:
	ContractMap<std::set<ContractKey >> _contract_strategy_map;
	SessionContainer_Ptr<ContractKey> _pricingNotifers;
	SessionContainer_Ptr<uint64_t> _otcOrderNotifers;

	CTPOTCTradeProcessor _otcTradeProcessor;

	IPricingDataContext* _pricingCtx;
private:
	


	//CTP APIs
public:

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

};

#endif