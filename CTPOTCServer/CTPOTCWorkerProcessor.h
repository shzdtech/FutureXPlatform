/***********************************************************************
 * Module:  CTPOTCWorkerProcessor.h
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Declaration of the class CTPOTCWorkerProcessor
 ***********************************************************************/

#if !defined(__CTP_CTPOTCWorkerProcessor_h)
#define __CTP_CTPOTCWorkerProcessor_h

#include <set>
#include <list>
#include "CTPOTCTradeProcessor.h"
#include "../ordermanager/OTCOrderManager.h"
#include "../CTPServer/CTPMarketDataProcessor.h"
#include "../dataobject/TypedefDO.h"

#include "../message/SessionContainer.h"

class CTPOTCWorkerProcessor : public CTPMarketDataProcessor
{
public:
   CTPOTCWorkerProcessor(const std::map<std::string, std::string>& configMap);
   ~CTPOTCWorkerProcessor();
   bool OnSessionClosing(void);
   void setSession(IMessageSession* msgSession);

   int LoginIfNeed(void);
  
   int RefreshStrategy(const StrategyContractDO& strategyDO);
   void AddContractToMonitor(const ContractKey& contractId);
  
   void TriggerPricing(const StrategyContractDO& strategyDO);
   void TriggerUpdating(const MarketDataDO& mdDO);
   void TriggerOTCOrderUpdating(const StrategyContractDO& strategyDO);
   void TriggerOrderUpdating(const StrategyContractDO& strategyDO);
   
   void RegisterPricingListener(const ContractKey& contractId,
	   IMessageSession* pMsgSession);
   void UnRegisterPricingListener(const ContractKey& contractId,
	   IMessageSession* pMsgSession);
   void RegisterOTCOrderListener(const int orderID,
	   IMessageSession* pMsgSession);
   void RegisterOrderListener(const int orderID,
	   IMessageSession* pMsgSession);

   void CancelAutoOrder(const UserContractKey& userContractKey);
   int OTCNewOrder(OrderDO& orderReq);
   int OTCCancelOrder(OrderDO& orderReq);

protected:
	ContractMap<std::set<ContractKey >> _contract_strategy_map;
	SessionContainer_Ptr<ContractKey> _pricingNotifers;
	SessionContainer_Ptr<uint64_t> _otcOrderNotifers;

	CTPOTCTradeProcessor _otcTradeProcessor;

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