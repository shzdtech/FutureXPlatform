/***********************************************************************
 * Module:  XTOTCTradeProcessor.h
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Declaration of the class XTOTCTradeProcessor
 ***********************************************************************/

#if !defined(__XTOTC_CTPOTCTradeProcessor_h)
#define __XTOTC_CTPOTCTradeProcessor_h

#include "XTTradeProcessor.h"
#include "../OTCServer/OTCTradeProcessor.h"
#include "../utility/lockfree_set.h"

#include "xt_export.h"

class XTOTCTradeWorkerProcessor;

class XT_CLASS_EXPORT XTOTCTradeProcessor : public XTTradeProcessor, public OTCTradeProcessor
{
public:
	XTOTCTradeProcessor();
	XTOTCTradeProcessor(const XTRawAPI_Ptr& rawAPI);
	~XTOTCTradeProcessor();

	virtual void OnTraded(const TradeRecordDO_Ptr & tradeDO);

	virtual OrderDO_Ptr CreateOrder(const OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelOrder(const OrderRequestDO& orderReq);
	virtual uint32_t GetSessionId(void);

	virtual OTCOrderManager& GetOTCOrderManager(void);
	virtual AutoOrderManager& GetAutoOrderManager(void);
	virtual HedgeOrderManager& GetHedgeOrderManager(void);

protected:
	XTOTCTradeWorkerProcessor * getWorkerProcessor();

private:
};

typedef std::shared_ptr<XTOTCTradeProcessor> XTOTCTradeProcessor_Ptr;

#endif