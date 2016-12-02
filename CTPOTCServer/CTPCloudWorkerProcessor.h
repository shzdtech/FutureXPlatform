/***********************************************************************
 * Module:  CTPCloudWorkerProcessor.h
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Declaration of the class CTPCloudWorkerProcessor
 ***********************************************************************/

#if !defined(__CTPOTC_CTPCloudWorkerProcessor_h)
#define __CTPOTC_CTPCloudWorkerProcessor_h

#include <set>
#include <list>
#include "CTPOTCTradeProcessor.h"
#include "../ordermanager/OTCOrderManager.h"
#include "../CTPServer/CTPMarketDataProcessor.h"
#include "../dataobject/TypedefDO.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/SessionContainer.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPCloudWorkerProcessor : public OTCWorkerProcessor, public CTPMarketDataProcessor
{
public:
	CTPCloudWorkerProcessor(IServerContext* pServerCtx, const std::shared_ptr<CTPOTCTradeProcessor>& ctpOtcTradeProcessorPtr);

	~CTPCloudWorkerProcessor();

	void setSession(const IMessageSession_WkPtr& msgSession_wk_ptr);
	bool OnSessionClosing(void);

	void Initialize(IServerContext* pServerCtx);

	int LoginSystemUserIfNeed(void);

	int SubscribeMarketData(const ContractKey & contractId);

	void RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr);

	ProductType GetContractProductType() const;

	const std::vector<ProductType>& GetStrategyProductTypes() const;

	OTCTradeProcessor* GetOTCTradeProcessor();

protected:
	std::shared_ptr<CTPOTCTradeProcessor> _ctpOtcTradeProcessorPtr;

private:



	//CTP APIs
public:

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};

#endif