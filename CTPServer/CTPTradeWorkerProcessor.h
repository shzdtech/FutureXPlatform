/***********************************************************************
 * Module:  CTPTradeWorkerProcessor.h
 * Author:  milk
 * Modified: 2016年6月16日 11:57:29
 * Purpose: Declaration of the class CTPTradeWorkerProcessor
 ***********************************************************************/

#if !defined(__CTPServer_CTPTradeWorkerProcessor_h)
#define __CTPServer_CTPTradeWorkerProcessor_h

#include "CTPTradeProcessor.h"
#include <future>
#include "../message/UserInfo.h"
#include "../message/SessionContainer.h"
#include "../ordermanager/IOrderAPI.h"
#include "../utility/autofillmap.h"
#include "../dataobject/AccountInfoDO.h"
#include "../dataobject/ExchangeDO.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/OrderDO.h"

#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPTradeWorkerProcessor : public CTPTradeProcessor, public IOrderAPI
{
public:
   CTPTradeWorkerProcessor(const std::map<std::string, std::string>& configMap);
   ~CTPTradeWorkerProcessor();
   virtual void Initialize(void);
   virtual int LoadSystemData(CThostFtdcTraderApi* trdAPI);
   virtual int LoginSystemUser(void);
   virtual int LoginSystemUserIfNeed(void);

   virtual int CreateOrder(const OrderDO& orderInfo, OrderStatus& currStatus);
   virtual int CancelOrder(const OrderDO& orderInfo, OrderStatus& currStatus);

   virtual void RegisterLoggedSession(IMessageSession* pMessageSession);
   virtual void DispatchUserMessage(int msgId, const std::string& userId, const dataobj_ptr& dataobj_ptr);
   virtual std::vector<AccountInfoDO>& GetAccountInfo(const std::string userId);
   virtual std::set<ExchangeDO>& GetExchangeInfo();
   virtual UserPositionExDOMap& GetUserPositionMap();
   virtual autofillmap<uint64_t, TradeRecordDO>& GetUserTradeMap(const std::string userId);
   virtual autofillmap<uint64_t, OrderDO>& GetUserOrderMap(const std::string userId);

   int RetryInterval = 60000;
   bool DataLoaded;

protected:
   UserInfo _systemUser;
   std::future<void> _initializer;
   SessionContainer_Ptr<std::string> _userSessionCtn_Ptr;
   autofillmap<std::string, std::vector<AccountInfoDO>> _accountInfoMap;
   UserPositionExDOMap _userPositionMap;
   std::set<ExchangeDO> _exchangeInfo_Set;
   autofillmap<std::string, autofillmap<uint64_t, TradeRecordDO>> _userTradeMap;
   autofillmap<std::string, autofillmap<uint64_t, OrderDO>> _userOrderMap;


public:
	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///报单录入错误回报
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	///报单操作错误回报
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
};

#endif