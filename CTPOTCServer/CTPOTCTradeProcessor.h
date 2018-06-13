/***********************************************************************
 * Module:  CTPOTCTradeProcessor.h
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Declaration of the class CTPOTCTradeProcessor
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCTradeProcessor_h)
#define __CTPOTC_CTPOTCTradeProcessor_h

#include "../CTPServer/CTPTradeProcessor.h"
#include "../OTCServer/OTCTradeProcessor.h"
#include "../utility/lockfree_set.h"

#include "ctpotc_export.h"

class CTPOTCTradeWorkerProcessor;

class CTP_OTC_CLASS_EXPORT CTPOTCTradeProcessor : public CTPTradeProcessor, public OTCTradeProcessor
{
public:
	CTPOTCTradeProcessor();
	CTPOTCTradeProcessor(const CTPRawAPI_Ptr& rawAPI);
	~CTPOTCTradeProcessor();

	virtual void OnTraded(const TradeRecordDO_Ptr & tradeDO);

	virtual OrderDO_Ptr CreateOrder(const OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelOrder(const OrderRequestDO& orderReq);
	virtual uint32_t GetSessionId(void);

	virtual OTCOrderManager& GetOTCOrderManager(void);
	virtual AutoOrderManager& GetAutoOrderManager(void);
	virtual HedgeOrderManager& GetHedgeOrderManager(void);

protected:
	CTPOTCTradeWorkerProcessor * getWorkerProcessor();

private:
};

typedef std::shared_ptr<CTPOTCTradeProcessor> CTPOTCTradeProcessor_Ptr;

#endif