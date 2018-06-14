/***********************************************************************
 * Module:  XTOTCTradeWorkerProcessor.h
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Declaration of the class XTOTCTradeWorkerProcessor
 ***********************************************************************/

#if !defined(__XTOTC_CTPOTCTradeWorkerProcessor_h)
#define __XTOTC_CTPOTCTradeWorkerProcessor_h

#include "../ordermanager/OTCOrderManager.h"
#include "../ordermanager/AutoOrderManager.h"
#include "../ordermanager/HedgeOrderManager.h"
#include "../message/SessionContainer.h"
#include "../dataobject/StrategyContractDO.h"
#include "../OTCServer/OTCTradeWorkerProcessor.h"
#include "../pricingengine/PricingDataContext.h"
#include "../utility/lockfree_set.h"
#include "../CTPOTCServer/CTPOTCTradeWorkerProcessorBase.h"

#include "XTTradeWorkerProcessor.h"
#include "XTOTCTradeProcessor.h"

#include "xt_export.h"

class CTPOTCWorkerProcessor;

class XT_CLASS_EXPORT XTOTCTradeWorkerProcessor : public CTPOTCTradeWorkerProcessorBase, public XTTradeWorkerProcessor
{
public:
	XTOTCTradeWorkerProcessor(IServerContext* pServerCtx, const IPricingDataContext_Ptr& pricingDataCtx, const IUserPositionContext_Ptr& positionCtx);
	~XTOTCTradeWorkerProcessor();

	virtual void OnTraded(const TradeRecordDO_Ptr& tradeDO);

	virtual OrderDO_Ptr CreateOrder(const OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelOrder(const OrderRequestDO& orderReq);
	virtual OrderDO_Ptr CancelAutoOrder(const UserContractKey & userContractKey);
	virtual OrderDO_Ptr CancelHedgeOrder(const PortfolioKey & portfolioKey);
	virtual uint32_t GetSessionId(void);

	virtual SessionContainer_Ptr<std::string>& GetUserSessionContainer(void);
	virtual IUserPositionContext_Ptr& GetUserPositionContext();
	virtual UserTradeContext& GetUserTradeContext();
	virtual 
	virtual UserOrderContext& GetExchangeOrderContext(void);

protected:
	virtual bool& Closing(void);

public:

	///XT API;
	virtual void onRtnOrderDetail(const COrderDetail* data) {}
	// 获得主推的成交明细
	virtual void onRtnDealDetail(const CDealDetail* data) {}
	// 获得主推的委托错误信息
	virtual void onRtnOrderError(const COrderError* data) {}
	// 获得主推的撤销信息
	virtual void onRtnCancelError(const CCancelError* data) {}
};

#endif