/***********************************************************************
 * Module:  CTPNewOrder.cpp
 * Author:  milk
 * Modified: 2015年10月20日 22:38:54
 * Purpose: Implementation of the class CTPNewOrder
 ***********************************************************************/

#include "CTPNewOrder.h"
#include "CTPRawAPI.h"
#include "CTPUtility.h"
#include "CTPMapping.h"
#include "CTPConstant.h"
#include "CTPTradeWorkerProcessor.h"

#include "../ordermanager/OrderSeqGen.h"
#include "../message/message_macro.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"

#include "../ordermanager/OrderPortfolioCache.h"

#include "../dataobject/OrderDO.h"



 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPNewOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
 // Purpose:    Implementation of CTPNewOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPNewOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	CheckAllowTrade(session);

	auto pDO = (OrderRequestDO*)reqDO.get();

	auto& userInfo = session->getUserInfo();

	// 端登成功,发出报单录入请求
	CThostFtdcInputOrderField req{};

	//经纪公司代码
	std::strncpy(req.BrokerID, userInfo.getBrokerId().data(), sizeof(req.BrokerID));
	//投资者代码
	std::strncpy(req.InvestorID, userInfo.getInvestorId().data(), sizeof(req.InvestorID));
	// 合约代码
	std::strncpy(req.InstrumentID, pDO->InstrumentID().data(), sizeof(req.InstrumentID));
	///报单引用
	pDO->OrderID = OrderSeqGen::GenOrderID(userInfo.getSessionId());

	std::snprintf(req.OrderRef, sizeof(req.OrderRef), FMT_ORDERREF, pDO->OrderID);
	// 用户代码
	pDO->SetUserID(userInfo.getUserId());
	std::strncpy(req.UserID, pDO->UserID().data(), sizeof(req.UserID));
	// 报单价格条件
	auto it = CTPExecPriceMapping.find((OrderExecType)pDO->ExecType);
	req.OrderPriceType = it != CTPExecPriceMapping.end() ? it->second : THOST_FTDC_OPT_LimitPrice;
	// 买卖方向
	req.Direction = pDO->Direction > 0 ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;
	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open + pDO->OpenClose;
	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	// 价格
	req.LimitPrice = pDO->LimitPrice;
	// 数量
	req.VolumeTotalOriginal = pDO->Volume;
	// 有效期类型
	auto tit = CTPTIFMapping.find((OrderTIFType)pDO->TIF);
	req.TimeCondition = tit != CTPTIFMapping.end() ? tit->second : THOST_FTDC_TC_GFD;
	// GTD日期
	//std::strcpy(req.GTDDate, "");
	// 成交量类型
	auto vit = CTPVolCondMapping.find((OrderVolType)pDO->VolCondition);
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

	bool insertPortfolio = !pDO->PortfolioID().empty();

	if (insertPortfolio)
	{
		OrderPortfolioCache::Insert(pDO->OrderID, *pDO);
	}

	int iRet = ((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqOrderInsert(&req, serialId);

	if (iRet != 0 && insertPortfolio)
	{
		OrderPortfolioCache::Remove(pDO->OrderID);
	}

	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPNewOrder::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of CTPNewOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPNewOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	OrderDO_Ptr ret;

	if (auto pData = (CThostFtdcInputOrderField*)rawRespParams[0])
	{
		auto pRsp = (CThostFtdcRspInfoField*)rawRespParams[1];
		if (ret = CTPUtility::ParseRawOrder(pData, pRsp, session->getUserInfo().getSessionId()))
		{
			if (ret->IsSystemUserId())
			{
				ret->SetUserID(session->getUserInfo().getUserId());
			}
		}

		ret->HasMore = false;
	}
	else
	{
		CTPUtility::CheckError(rawRespParams[1]);
	}

	return ret;
}