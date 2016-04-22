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
#include "../ordermanager/OrderSeqGen.h"

#include "../dataobject/OrderDO.h"
#include "tradeapi/ThostFtdcTraderApi.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPNewOrder::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPNewOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPNewOrder::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto pDO = (OrderDO*)reqDO.get();

	auto userinfo = session->getUserInfo();

	// 端登成功,发出报单录入请求
	CThostFtdcInputOrderField req;
	std::memset(&req, 0, sizeof(req));

	//经纪公司代码
	std::strcpy(req.BrokerID, userinfo->getBrokerId().data());
	//投资者代码
	std::strcpy(req.InvestorID, userinfo->getInvestorId().data());
	// 合约代码
	std::strcpy(req.InstrumentID, pDO->InstrumentID().data());
	///报单引用
	uint64_t orderID = pDO->OrderID == 0 ? OrderSeqGen::GetNextSeq() : pDO->OrderID;
	std::sprintf(req.OrderRef, FMT_PADDING_ORDERREF, orderID);
	// 用户代码
	std::strcpy(req.UserID, userinfo->getUserId().data());
	// 报单价格条件
	req.OrderPriceType = CTPExecPriceMapping.at((OrderExecType)pDO->ExecType);
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
	req.TimeCondition = CTPTIFMapping.at((OrderTIFType)pDO->TIF);
	// GTD日期
	std::strcpy(req.GTDDate, "");
	// 成交量类型
	req.VolumeCondition = THOST_FTDC_VC_AV;
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

	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqOrderInsert(&req, ++_requestIdGen);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPNewOrder::HandleResponse(ParamVector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPNewOrder::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPNewOrder::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	dataobj_ptr ret;

	if (rawRespParams.size() > 1)
	{
		auto pRsp = (CThostFtdcRspInfoField*)rawRespParams[1];
		auto pData = (CThostFtdcInputOrderField*)rawRespParams[0];
		auto pDO = new OrderDO(std::strtoull(pData->OrderRef, nullptr, 0),
			EXCHANGE_EMPTY, pData->InstrumentID, pData->UserID);
		ret.reset(pDO);
		pDO->Direction = (pData->Direction == THOST_FTDC_D_Buy) ? DirectionType::BUY : DirectionType::SELL;
		pDO->LimitPrice = pData->LimitPrice;
		pDO->Volume = pData->VolumeTotalOriginal;
		pDO->StopPrice = pData->StopPrice;
		pDO->ErrorCode = pRsp->ErrorID;
		pDO->OrderStatus = OrderStatus::OPEN_REJECTED;
	}
	else
	{
		auto pData = (CThostFtdcOrderField*)rawRespParams[0];
		auto pDO = new OrderDO(std::strtoull(pData->OrderRef, nullptr, 0),
			pData->ExchangeID, pData->InstrumentID, pData->UserID);
		ret.reset(pDO);
		pDO->Direction = (pData->Direction == THOST_FTDC_D_Buy) ? DirectionType::BUY : DirectionType::SELL;
		pDO->LimitPrice = pData->LimitPrice;
		pDO->Volume = pData->VolumeTotalOriginal;
		pDO->StopPrice = pData->StopPrice;
		pDO->TradingDay = std::strtoull(pData->TradingDay, nullptr, 0);
		pDO->OrderStatus = CTPUtility::CheckOrderStatus(pData->OrderStatus, pData->OrderSubmitStatus);
	}

	return ret;
}