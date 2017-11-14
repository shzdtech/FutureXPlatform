/***********************************************************************
 * Module:  CTPOTCWorkerProcessor.h
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Declaration of the class CTPOTCWorkerProcessor
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCOptionWorkerProcessor_h)
#define __CTPOTC_CTPOTCOptionWorkerProcessor_h


#include "CTPOTCWorkerProcessor.h"
#include <thread>
#include "ctpotc_export.h"


class CTP_OTC_CLASS_EXPORT CTPOTCOptionWorkerProcessor : public CTPOTCWorkerProcessor
{
public:
	CTPOTCOptionWorkerProcessor(IServerContext* pServerCtx,
		const std::shared_ptr<CTPOTCTradeWorkerProcessor>& CTPOTCTradeWorkerProcessorPtr);

	~CTPOTCOptionWorkerProcessor();

	virtual ProductType GetContractProductType() const;

	virtual InstrumentCache & GetInstrumentCache();

	virtual bool TriggerTradingDeskParams(const StrategyContractDO & strategyDO);

	// CTP API
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField * pDepthMarketData);

protected:


private:

};

#endif