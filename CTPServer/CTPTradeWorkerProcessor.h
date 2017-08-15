/***********************************************************************
 * Module:  CTPTradeWorkerProcessor.h
 * Author:  milk
 * Modified: 2016年6月16日 11:57:29
 * Purpose: Declaration of the class CTPTradeWorkerProcessor
 ***********************************************************************/

#if !defined(__CTPServer_CTPTradeWorkerProcessor_h)
#define __CTPServer_CTPTradeWorkerProcessor_h

#include "CTPTradeProcessor.h"
#include "../message/MessageWorkerProcessor.h"
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

class CTP_CLASS_EXPORT CTPTradeWorkerProcessor : public MessageWorkerProcessor, public CTPTradeProcessor
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

	virtual void DispatchUserMessage(int msgId, int serialId, const std::string& userId, const dataobj_ptr& dataobj_ptr);
	virtual autofillmap<std::string, AccountInfoDO>& GetAccountInfo(const std::string userId);
	virtual std::set<ExchangeDO>& GetExchangeInfo();
	virtual IUserPositionContext_Ptr& GetUserPositionContext();
	virtual UserTradeContext& GetUserTradeContext();
	virtual UserOrderContext& GetUserOrderContext();
	virtual std::set<ProductType>& GetProductTypeToLoad();
	//virtual UserOrderContext& GetUserErrOrderContext(void);

	int ComparePosition(autofillmap<std::pair<std::string, PositionDirectionType>, std::pair<int, int>>& positions);

	int SyncPosition(const std::string& userId);

	virtual TradeRecordDO_Ptr RefineTrade(CThostFtdcTradeField * pTrade);

	virtual UserPositionExDO_Ptr UpdatePosition(const TradeRecordDO_Ptr& trdDO_Ptr);

	virtual OrderDO_Ptr CTPTradeWorkerProcessor::RefineOrder(CThostFtdcOrderField *pOrder);

	bool IsLoadPositionFromDB();

	void LoadPositonFromDatabase(const std::string& sysuserid, const std::string& tradingday);

	int RetryInterval = 30000;

protected:
	std::string _authCode;
	std::string _productInfo;

	std::mutex _loginMutex;
	autofillmap<std::string, autofillmap<std::string, AccountInfoDO>> _accountInfoMap;
	std::set<ExchangeDO> _exchangeInfo_Set;
	IUserPositionContext_Ptr _userPositionCtx_Ptr;
	UserTradeContext _userTradeCtx;
	UserOrderContext _userOrderCtx;
	//UserOrderContext _userErrOrderCtx;
	std::set<ProductType> _productTypes;

	std::future<void> _initializer;

	bool _loadPositionFromDB;

	autofillmap<std::pair<std::string, PositionDirectionType>, int> _ydDBPositions;
	autofillmap<std::pair<std::string, PositionDirectionType>, UserPositionExDO_Ptr> _ydSysPositions;

public:
	virtual void OnFrontConnected();
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);
	///客户端认证响应
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
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