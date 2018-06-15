/***********************************************************************
 * Module:  XTNewOrder.cpp
 * Author:  milk
 * Modified: 2015年10月20日 22:38:54
 * Purpose: Implementation of the class XTNewOrder
 ***********************************************************************/

#include "XTNewOrder.h"
#include "XTRawAPI.h"
#include "XTUtility.h"
#include "XTMapping.h"
#include "XTConstant.h"
#include "XTTradeWorkerProcessor.h"

#include "../ordermanager/OrderSeqGen.h"
#include "../message/message_macro.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"

#include "../ordermanager/OrderPortfolioCache.h"

#include "../dataobject/OrderDO.h"
#include "../riskmanager/RiskUtil.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       XTNewOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
 // Purpose:    Implementation of XTNewOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr XTNewOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	CheckAllowTrade(session);
	XTUtility::CheckTradeInit((XTRawAPI*)rawAPI);

	auto pOrderReqDO = (OrderRequestDO*)reqDO.get();
	pOrderReqDO->TradingType = OrderTradingType::TRADINGTYPE_MANUAL;

	auto& userInfo = session->getUserInfo();

	pOrderReqDO->SetUserID(userInfo.getUserId());

	std::shared_ptr<BizException> bizEx_Ptr;
	// Pretrade risk check
	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessorBase>(msgProcessor))
	{
		try
		{
			RiskUtil::CheckRisk(*pOrderReqDO, pWorkerProc->GetUserPositionContext().get(), pWorkerProc->GetAccountInfo(pOrderReqDO->UserID()).get());
		}
		catch (BizException& ex)
		{
			if (ex.ErrorCode() > RiskModelParams::WARNING)
				throw ex;

			if (ex.ErrorCode() > RiskModelParams::NO_ACTION)
				bizEx_Ptr = std::make_shared<BizException>(ex);
		}
	}

	// 端登成功,发出报单录入请求
	CThostFtdcInputOrderField req{};

	//经纪公司代码
	std::strncpy(req.BrokerID, userInfo.getBrokerId().data(), sizeof(req.BrokerID));
	//投资者代码
	std::strncpy(req.InvestorID, userInfo.getInvestorId().data(), sizeof(req.InvestorID));
	// 合约代码
	std::strncpy(req.InstrumentID, pOrderReqDO->InstrumentID().data(), sizeof(req.InstrumentID));
	///报单引用
	pOrderReqDO->OrderID = OrderSeqGen::GenOrderID(userInfo.getSessionId());

	std::snprintf(req.OrderRef, sizeof(req.OrderRef), FMT_ORDERREF, pOrderReqDO->OrderID);
	// 用户代码
	std::strncpy(req.UserID, pOrderReqDO->UserID().data(), sizeof(req.UserID));
	// 报单价格条件
	auto it = CTPExecPriceMapping.find((OrderExecType)pOrderReqDO->ExecType);
	req.OrderPriceType = it != CTPExecPriceMapping.end() ? it->second : THOST_FTDC_OPT_LimitPrice;
	// 买卖方向
	req.Direction = pOrderReqDO->Direction > 0 ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;
	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open + pOrderReqDO->OpenClose;
	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	// 价格
	req.LimitPrice = pOrderReqDO->LimitPrice;
	// 数量
	req.VolumeTotalOriginal = pOrderReqDO->Volume;
	// 有效期类型
	auto tit = CTPTIFMapping.find((OrderTIFType)pOrderReqDO->TIF);
	req.TimeCondition = tit != CTPTIFMapping.end() ? tit->second : THOST_FTDC_TC_GFD;
	// GTD日期
	//std::strcpy(req.GTDDate, "");
	// 成交量类型
	auto vit = CTPVolCondMapping.find((OrderVolType)pOrderReqDO->VolCondition);
	req.VolumeCondition = vit != CTPVolCondMapping.end() ? vit->second : THOST_FTDC_VC_AV;
	// 最小成交量
	req.MinVolume = 1;
	// 触发条件
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	// 止损价
	req.StopPrice = 0;
	// 强平原因
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	// 自动挂起标志
	req.IsAutoSuspend = false;

	req.RequestID = serialId;

	bool insertPortfolio = !pOrderReqDO->PortfolioID().empty();

	if (insertPortfolio)
	{
		OrderPortfolioCache::Insert(pOrderReqDO->OrderID, *pOrderReqDO);
	}

	((XTRawAPI*)rawAPI)->get()->order();


	if (bizEx_Ptr)
		throw *bizEx_Ptr;

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTNewOrder::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of XTNewOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr XTNewOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	OrderDO_Ptr ret;

	if (auto pData = (CThostFtdcInputOrderField*)rawRespParams[0])
	{
		if (session->getUserInfo().getUserId() == pData->UserID)
		{
			auto pRsp = (CThostFtdcRspInfoField*)rawRespParams[1];

			if (ret = XTUtility::ParseRawOrder(pData, pRsp, session->getUserInfo().getSessionId()))
			{
				ret->HasMore = false;
			}
		}
	}
	else
	{
		XTUtility::CheckError(rawRespParams[1]);
	}

	return ret;
}