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
#include "../CTPServer/CTPMarketDataSAProcessor.h"
#include "../dataobject/TypedefDO.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/SessionContainer.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCWorkerProcessor : public OTCWorkerProcessor, public CTPMarketDataSAProcessor
{
public:
	CTPOTCWorkerProcessor(IServerContext* pServerCtx, const std::shared_ptr<CTPOTCTradeProcessor>& ctpOtcTradeProcessorPtr);

	~CTPOTCWorkerProcessor();

	void setMessageSession(const IMessageSession_Ptr& msgSession_ptr);

	void setServiceLocator(const IMessageServiceLocator_Ptr & svcLct_Ptr);

	bool OnSessionClosing(void);

	void Initialize(IServerContext* pServerCtx);

	int SubscribeMarketData(const ContractKey & contractId);

	void RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr);

	ProductType GetContractProductType() const;

	const std::vector<ProductType>& GetStrategyProductTypes() const;

	OTCTradeProcessor* GetOTCTradeProcessor();

	CTPOTCTradeProcessor* GetCTPOTCTradeProcessor();

	int ResubMarketData(void);

protected:
	std::shared_ptr<CTPOTCTradeProcessor> _ctpOtcTradeProcessorPtr;
	std::future<void> _initializer;
	UserInfo _systemUser;
	int RetryInterval = 60000;
	volatile bool _closing = false;

private:



	//CTP APIs
public:

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

};

#endif