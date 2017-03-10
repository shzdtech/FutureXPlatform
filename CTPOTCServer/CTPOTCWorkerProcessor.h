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
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/SessionContainer.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCWorkerProcessor : public OTCWorkerProcessor, public CTPMarketDataProcessor
{
public:
	CTPOTCWorkerProcessor(IServerContext* pServerCtx, const std::shared_ptr<CTPOTCTradeProcessor>& ctpOtcTradeProcessorPtr);

	~CTPOTCWorkerProcessor();

	void setMessageSession(const IMessageSession_Ptr& msgSession_ptr);

	bool OnSessionClosing(void);

	void Initialize(IServerContext* pServerCtx);

	int LoginSystemUser(void);

	int LoginSystemUserIfNeed(void);

	int LoadDataAsync(void);

	int SubscribeMarketData(const ContractKey & contractId);

	void RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr);

	ProductType GetContractProductType() const;

	const std::vector<ProductType>& GetStrategyProductTypes() const;

	OTCTradeProcessor* GetOTCTradeProcessor();

protected:
	std::shared_ptr<CTPOTCTradeProcessor> _ctpOtcTradeProcessorPtr;
	std::thread _initializer;
	UserInfo _systemUser;
	int RetryInterval = 60000;
	volatile bool _closing = false;

private:



	//CTP APIs
public:

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField * pRspUserLogin, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

};

#endif