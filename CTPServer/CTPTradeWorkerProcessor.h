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
#include "../ordermanager/PortfolioPositionContext.h"
#include "../utility/autofillmap.h"
#include "../utility/hash_tuple.h"
#include "../dataobject/AccountInfoDO.h"
#include "../dataobject/ExchangeDO.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/OrderDO.h"
#include "../dataobject/EnumTypes.h"
#include "../bizutility/ManualOpHub.h"
#include "../utility/lockfree_queue.h"

#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPTradeWorkerProcessor : public MessageWorkerProcessor, public CTPTradeProcessor, public IManualOp
{
public:
	CTPTradeWorkerProcessor(IServerContext* pServerCtx, const IUserPositionContext_Ptr& positionCtx = nullptr);
	~CTPTradeWorkerProcessor();

	virtual void OnNewManualTrade(const TradeRecordDO& tradeDO);
	virtual void OnUpdateManualPosition(const UserPositionExDO& positionDO);


	virtual void Initialize(IServerContext* pServerCtx);
	virtual int RequestData(void);
	virtual int LoginSystemUser(void);
	virtual int LoginSystemUserIfNeed(void);
	virtual int LoadContractFromDB(void);
	virtual int LoadDataAsync(void);
	virtual void OnDataLoaded(void);

	virtual void DispatchUserMessage(int msgId, int serialId, const std::string& userId, const dataobj_ptr& dataobj_ptr);
	virtual void DispatchMessageForAll(int msgId, int serialId, const dataobj_ptr & dataobj_ptr);
	virtual AccountInfoDO_Ptr GetAccountInfo(const std::string& userId);
	virtual void UpdateAccountInfo(const std::string& userId, const AccountInfoDO_Ptr& accoutInfo);
	virtual std::set<ExchangeDO>& GetExchangeInfo();
	virtual IUserPositionContext_Ptr& GetUserPositionContext();
	virtual UserTradeContext& GetUserTradeContext();
	virtual UserOrderContext& GetUserOrderContext();
	virtual std::set<ProductType>& GetProductTypeToLoad();
	//virtual UserOrderContext& GetUserErrOrderContext(void);

	int ComparePosition(const std::string& userId, autofillmap<std::tuple<std::string, std::string, PositionDirectionType>, std::pair<int, int>>& positions);

	virtual TradeRecordDO_Ptr RefineTrade(CThostFtdcTradeField * pTrade);

	virtual UserPositionExDO_Ptr UpdatePosition(const TradeRecordDO_Ptr& trdDO_Ptr);

	virtual OrderDO_Ptr CTPTradeWorkerProcessor::RefineOrder(CThostFtdcOrderField *pOrder);

	void LoadPositonFromDatabase(const std::string & userId, const std::string& tradingday);

	void UpdateSysYdPosition(const std::string & userId, const UserPositionExDO_Ptr & position_ptr);

	UserPositionExDO_Ptr FindDBYdPostion(const std::string& userid, const std::string& contract, const std::string& portfolio, PositionDirectionType direction);

	UserPositionExDO_Ptr FindSysYdPostion(const std::string& userid, const std::string& contract, PositionDirectionType direction);

	void PushToLogQueue(const TradeRecordDO_Ptr& tradeDO_Ptr);

	void LogTrade();

	bool IsLoadPositionFromDB();

	bool IsSaveTrade();

	int RetryInterval = 30000;
	int RetryTimes = 10;

protected:
	std::string _authCode;
	std::string _productInfo;

	std::mutex _loginMutex;
	std::set<ExchangeDO> _exchangeInfo_Set;
	IUserPositionContext_Ptr _userPositionCtx_Ptr;
	UserTradeContext _userTradeCtx;
	UserOrderContext _userOrderCtx;
	//UserOrderContext _userErrOrderCtx;
	std::set<ProductType> _productTypes;

	std::future<void> _initializer;

	cuckoohash_map<std::string, AccountInfoDO_Ptr> _accountInfoMap;

	typedef cuckoohashmap_wrapper<std::tuple<std::string, std::string, PositionDirectionType>, UserPositionExDO_Ptr, 
		std::hash<std::tuple<std::string, std::string, PositionDirectionType>>> Position_HashMap;

	typedef cuckoohashmap_wrapper<std::pair<std::string, PositionDirectionType>, UserPositionExDO_Ptr,
		pairhash<std::string, PositionDirectionType>> SysPosition_HashMap;

	cuckoohash_map<std::string, Position_HashMap> _ydDBPositions;
	cuckoohash_map<std::string, SysPosition_HashMap> _ydSysPositions;

	bool _loadPositionFromDB = false;
	bool _logTrades = false;
	std::thread _tradeDBSerializer;
	lockfree_queue<TradeRecordDO_Ptr> _tradeQueue;

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