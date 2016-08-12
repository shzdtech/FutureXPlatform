/***********************************************************************
 * Module:  CTPOTCWorkerProcessor.h
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Declaration of the class CTPOTCWorkerProcessor
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCOptionWorkerProcessor_h)
#define __CTPOTC_CTPOTCOptionWorkerProcessor_h


#include "CTPOTCWorkerProcessor.h"

#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCOptionWorkerProcessor : public CTPOTCWorkerProcessor
{
public:
	CTPOTCOptionWorkerProcessor(const std::map<std::string, std::string>& configMap,
		IServerContext* pServerCtx,
		const std::shared_ptr<CTPOTCTradeProcessor>& ctpOtcTradeProcessorPtr);

	virtual void TriggerPricing(const StrategyContractDO& strategyDO);

	virtual ProductType GetProductType();

protected:

private:

};

#endif