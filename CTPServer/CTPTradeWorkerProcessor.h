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
#include "../dataobject/TypedefDO.h"

#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPTradeWorkerProcessor : public CTPTradeProcessor, public IOrderAPI
{
public:
   CTPTradeWorkerProcessor(const std::map<std::string, std::string>& configMap);
   ~CTPTradeWorkerProcessor();
   virtual void Initialize(void);
   virtual int LoginSystemUser(void);
   virtual int LoginSystemUserIfNeed(void);

   virtual int CreateOrder(const OrderDO& orderInfo, OrderStatus& currStatus);
   virtual int CancelOrder(const OrderDO& orderInfo, OrderStatus& currStatus);

   virtual void RegisterLoggedSession(IMessageSession* pMessageSession);
   virtual void DispatchUserMessage(int msgId, const std::string& userId, dataobj_ptr dataobj_ptr);
   virtual std::vector<AccountInfoDO>& GetAccountInfo(const std::string userId);
   virtual std::set<ExchangeDO>& GetExchangeInfo();
   virtual UserPositionExDOMap& GetUserPositionMap();

   int RetryInterval = 60000;
   bool InstrmentsLoaded;

protected:
   UserInfo _systemUser;
   std::future<void> _initializer;
   SessionContainer_Ptr<std::string> _userSessionCtn_Ptr;
   autofillmap<std::string, std::vector<AccountInfoDO>> _accountInfoMap;
   UserPositionExDOMap _userPositionMap;
   std::set<ExchangeDO> _exchangeInfo_Set;

public:
	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	
	///请求查询报单响应
	void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询成交响应
	void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};

#endif