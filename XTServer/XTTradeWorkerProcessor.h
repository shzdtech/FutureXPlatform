/***********************************************************************
 * Module:  XTTradeWorkerProcessor.h
 * Author:  milk
 * Modified: 2016年6月16日 11:57:29
 * Purpose: Declaration of the class XTTradeWorkerProcessor
 ***********************************************************************/

#if !defined(__XTServer_XTTradeWorkerProcessor_h)
#define __XTServer_XTTradeWorkerProcessor_h

#include "XTTradeProcessor.h"
#include "../CTPServer/CTPTradeWorkerProcessorBase.h"
#include "../ordermanager/IOrderAPI.h"
#include "../ordermanager/UserOrderContext.h"
#include "../ordermanager/UserTradeContext.h"
#include "../ordermanager/PortfolioPositionContext.h"
#include "../utility/autofillmap.h"
#include "../utility/hash_tuple.h"
#include "../dataobject/AccountInfoDO.h"
#include "../dataobject/ExchangeDO.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/OrderDO.h"
#include "../dataobject/EnumTypes.h"
#include "../bizutility/ManualOpHub.h"
#include "../utility/lockfree_queue.h"

#include "xt_export.h"

class XT_CLASS_EXPORT XTTradeWorkerProcessor : public CTPTradeWorkerProcessorBase, public XTTradeProcessor
{
public:
	XTTradeWorkerProcessor(IServerContext* pServerCtx, const IUserPositionContext_Ptr& positionCtx = nullptr);
	~XTTradeWorkerProcessor();

	virtual void Initialize(IServerContext* pServerCtx);


	virtual int RequestData(void);
	virtual int LoginSystemUser(void);
	virtual int LoginSystemUserIfNeed(void);
	virtual int LogoutSystemUser(void);
	virtual int& GetDataLoadMask(void);
	virtual TemplateMessageProcessor* GetTemplateProcessor(void);
	virtual IServerContext* getServerContext(void);

	virtual TradeRecordDO_Ptr RefineTrade(CDealDetail* pTrade);
	virtual OrderDO_Ptr XTTradeWorkerProcessor::RefineOrder(COrderDetail *pOrder);

	void QueryAccountWorker();

protected:

	bool _isQueryAccount = true;
	std::future<void> _accountQuery;

public:
	// 相应请求的回调函数
	// @param   nRequestId 和请求函数中 nRequestId 想对应
	// @param   error 请求函数的返回是否成功，如果失败有错误信息
	// @remark  函数名组成：以 on 为前缀，加上相应请求函数的函数名称（请求函数首字母大写）

	// 连接服务器的回调函数
	// @param success   服务器连接是否成
	// @param errorMsg  如果服务器连接失败，存储错误信息
	virtual void onConnected(bool success, const char* errorMsg);
	// 客户端用户登录的回调函数
	virtual void onUserLogin(const char* userName, const char* password, int nRequestId, const XtError& error);
	// /客户端用户登出的回调函数
	virtual void onUserLogout(const char* userName, const char* password, int nRequestId, const XtError& error);

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

#endif