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
#include "../ordermanager/UserOrderContext.h"
#include "../ordermanager/UserTradeContext.h"
#include "../ordermanager/UserPositionContext.h"
#include "../utility/autofillmap.h"
#include "../dataobject/AccountInfoDO.h"
#include "../dataobject/ExchangeDO.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/OrderDO.h"
#include "../dataobject/EnumTypes.h"

#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPTradeWorkerProcessor : public CTPTradeProcessor
{
public:
	CTPTradeWorkerProcessor(IServerContext* pServerCtx, const IUserPositionContext_Ptr& positionCtx = nullptr);
   ~CTPTradeWorkerProcessor();
   virtual void Initialize(IServerContext* pServerCtx);
   virtual int RequestData(void);
   virtual int LoginSystemUser(void);
   virtual int LoginSystemUserIfNeed(void);
   virtual int LoadDataAsync(void);
   virtual void OnDataLoaded(void);

   virtual void RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr);
   virtual void DispatchUserMessage(int msgId, int serialId, const std::string& userId, const dataobj_ptr& dataobj_ptr);
   virtual autofillmap<std::string, AccountInfoDO>& GetAccountInfo(const std::string userId);
   virtual std::set<ExchangeDO>& GetExchangeInfo();
   virtual IUserPositionContext_Ptr& GetUserPositionContext();
   virtual UserTradeContext& GetUserTradeContext();
   virtual UserOrderContext& GetUserOrderContext();
   virtual std::set<ProductType>& GetProductTypeToLoad();
   //virtual UserOrderContext& GetUserErrOrderContext(void);
   virtual const IUserInfo& GetSystemUser();

   int ComparePosition(autofillmap<std::pair<std::string, PositionDirectionType>, std::pair<int, int>>& positions);

   virtual TradeRecordDO_Ptr RefineTrade(CThostFtdcTradeField * pTrade);

   virtual void UpdatePosition(const TradeRecordDO_Ptr& trdDO_Ptr);

   virtual OrderDO_Ptr CTPTradeWorkerProcessor::RefineOrder(CThostFtdcOrderField *pOrder);

   bool IsLoadPositionFromDB();

   void LoadPositonFromDatabase(const std::string& sysuserid, const std::string& tradingday);

   int RetryInterval = 30000;

protected:
   UserInfo _systemUser;
   volatile bool _closing = false;
   std::future<void> _initializer;
   SessionContainer_Ptr<std::string> _userSessionCtn_Ptr;
   autofillmap<std::string, autofillmap<std::string, AccountInfoDO>> _accountInfoMap;
   std::set<ExchangeDO> _exchangeInfo_Set;
   IUserPositionContext_Ptr _userPositionCtx_Ptr;
   UserTradeContext _userTradeCtx;
   UserOrderContext _userOrderCtx;
   //UserOrderContext _userErrOrderCtx;
   std::set<ProductType> _productTypes;

   bool _loadPositionFromDB;

   autofillmap<std::pair<std::string, PositionDirectionType>, int> _ydDBPositions;
   autofillmap<std::pair<std::string, PositionDirectionType>, int> _ydSysPositions;

public:
	virtual void OnFrontConnected();
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

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};

#endif