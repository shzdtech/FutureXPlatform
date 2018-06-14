/***********************************************************************
 * Module:  CTPTradeWorkerProcessor.h
 * Author:  milk
 * Modified: 2016年6月16日 11:57:29
 * Purpose: Declaration of the class CTPTradeWorkerProcessor
 ***********************************************************************/

#if !defined(__CTPServer_CTPTradeWorkerProcessor_h)
#define __CTPServer_CTPTradeWorkerProcessor_h

#include "CTPTradeProcessor.h"
#include "CTPTradeWorkerSAProcessor.h"
#include "CTPTradeWorkerProcessorBase.h"
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

class CTP_CLASS_EXPORT CTPTradeWorkerProcessor : public CTPTradeWorkerProcessorBase, public CTPTradeProcessor
{
public:
	CTPTradeWorkerProcessor(IServerContext* pServerCtx, const IUserPositionContext_Ptr& positionCtx = nullptr);
	~CTPTradeWorkerProcessor();

	virtual void Initialize(IServerContext* pServerCtx);


	virtual int RequestData(void);
	virtual int LoginSystemUser(void);
	virtual int LoginSystemUserIfNeed(void);
	virtual int LogoutSystemUser(void);
	virtual int& GetDataLoadMask(void);
	virtual TemplateMessageProcessor* GetTemplateProcessor(void);
	virtual IServerContext* getServerContext(void);

	virtual CTPTradeWorkerSAProcessor_Ptr CreateSAProcessor();
	virtual void LoginUserSession(const CTPProcessor_Ptr& sessionPtr, 
		const std::string & brokerId, const std::string & investorId, const std::string & password, const std::string & serverName = "");
	void UpdateSharedUserInfo(const std::string& investorId, const CTPTradeWorkerSAProcessor_Ptr& proc_ptr);
	bool IsUserLogged(const std::string & userId);

	virtual TradeRecordDO_Ptr RefineTrade(CThostFtdcTradeField * pTrade);
	virtual OrderDO_Ptr CTPTradeWorkerProcessor::RefineOrder(CThostFtdcOrderField *pOrder);

	void QueryAccountWorker();

protected:
	cuckoohash_map<std::string, CTPTradeWorkerSAProcessor_Ptr> _sharedProcHub;
	SessionContainer_Ptr<std::string> _sharedSystemSessionHub;

	bool _isQueryAccount = true;
	std::future<void> _accountQuery;

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

	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField * pTradingAccount, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast);
};

#endif