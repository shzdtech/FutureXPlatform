/***********************************************************************
 * Module:  CTPMarketDataSAProcessor.h
 * Author:  milk
 * Modified: 2014年10月6日 22:17:16
 * Purpose: Declaration of the class CTPMarketDataSAProcessor
 ***********************************************************************/

#if !defined(__CTP_CTPMarketDataSAProcessor_h)
#define __CTP_CTPMarketDataSAProcessor_h

#include "CTPMarketDataProcessor.h"
#include "../message/MessageWorkerProcessor.h"
#include "../utility/lockfree_set.h"
#include "../dataobject/TypedefDO.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPMarketDataSAProcessor : public MessageWorkerProcessor, public CTPMarketDataProcessor {
public:
	CTPMarketDataSAProcessor(IServerContext* pServerCtx);
	~CTPMarketDataSAProcessor();

	void Initialize(IServerContext * pServerCtx);

	virtual int LoginSystemUser(void);
	int LoginSystemUserIfNeed(void);
	virtual int ResubMarketData();
	virtual void DispatchUserMessage(int msgId, int serialId, const std::string& userId, const dataobj_ptr& dataobj_ptr);
	MarketDataDOMap& GetMarketDataMap();

public:
	SessionContainer_Ptr<std::string> MarketDataNotifers;

protected:
	std::future<void> _initializer;

	std::mutex _loginMutex;

	int RetryInterval = 30000;

	MarketDataDOMap _marketDataMap;

public:
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


};

#endif