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
	std::shared_ptr<XTOTCTradeProcessor> Shared_This();
	XTOTCTradeWorkerProcessor * getWorkerProcessor();

private:


public:
	// 下单的回调
	virtual void onOrder(int nRequestId, int orderID, const XtError& error);

	virtual void onCancelOrder(int nRequestId, const XtError& error);

	// 请求当前数据的回调函数
	// @param data 请求数据的返回
	// @param isLast 请求数据可能有多条，需要多次回调该函数，标记是否是一次请求的最后一次回调
	virtual void onReqAccountDetail(const char* accountID, int nRequestId, const CAccountDetail* data, bool isLast, const XtError& error);

	virtual void onReqPositionDetail(const char* accountID, int nRequestId, const CPositionDetail* data, bool isLast, const XtError& error);


	// 主推接口
	// 获得主推的委托明细（委托）
	virtual void onRtnOrderDetail(const COrderDetail* data);
	// 获得主推的成交明细
	virtual void onRtnDealDetail(const CDealDetail* data);
	// 获得主推的委托错误信息
	virtual void onRtnOrderError(const COrderError* data);
	// 获得主推的撤销信息
	virtual void onRtnCancelError(const CCancelError* data);
};

typedef std::shared_ptr<XTOTCTradeProcessor> XTOTCTradeProcessor_Ptr;

#endif