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

#include "../ordermanager/OrderReqCache.h"

#include "../dataobject/OrderDO.h"
#include "../riskmanager/RiskUtil.h"
#include "../bizutility/ContractCache.h"

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
	pOrderReqDO->SessionID = userInfo.getSessionId();
	pOrderReqDO->SerialId = serialId;
	pOrderReqDO->OrderID = OrderSeqGen::GenOrderID();

	ProductType productType;
	InstrumentCache& cache = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE);
	if (auto* pInstrument = cache.QueryInstrumentOrAddById(pOrderReqDO->InstrumentID()))
	{
		productType = pInstrument->ProductType;
		if (pOrderReqDO->ExchangeID().empty())
			pOrderReqDO->setExchangeID(pInstrument->ExchangeID());
	}

	COrdinaryOrder orderReq{};

	// 资金账号，必填参数。不填会被api打回，并且通过onOrder反馈失败
	std::strncpy(orderReq.m_strAccountID, userInfo.getInvestorId().data(), sizeof(orderReq.m_strAccountID));

	// 单笔超价百分比，选填字段。默认为0
	orderReq.m_dSuperPriceRate = 0;

	// 报单市场。必填字段。股票市场有"CFFEX"/"SHFE"/"DCE"/"CZCE"，如果填空或填错都会被api直接打回
	std::strncpy(orderReq.m_strMarket, pOrderReqDO->ExchangeID().data(), sizeof(orderReq.m_strMarket));

	// 报单合约代码，必填字段。
	std::strncpy(orderReq.m_strInstrument, pOrderReqDO->InstrumentID().data(), sizeof(orderReq.m_strInstrument));

	// 报单委托量，必填字段。默认int最大值，填0或不填会被api打回
	orderReq.m_nVolume = pOrderReqDO->Volume;

	// 报单委托类型。必填字段。根据相应的业务选择，具体请参考XtDataType.h，默认为无效值(OPT_INVALID)。不填会被api打回
	orderReq.m_eOperationType = XTUtility::GetOperationType(productType, pOrderReqDO->Direction, pOrderReqDO->OpenClose);

	// 报单价格类型，必填字段。默认为无效(PTRP_INVALID)，具体可参考XtDataType.h
	orderReq.m_ePriceType = PRTP_HANG;

	// 报单价格，默认为double最大值。当价格类型m_ePriceType为指定价PRTP_FIX时，必填字段。当价格类型为其他时填了也没用
	orderReq.m_dPrice = pOrderReqDO->LimitPrice;

	// 投机套保标志，选填字段。有"投机"/"套利"/"套保"方式。除期货三个方式都可选之外都是填“投机”。默认为“投机”
	orderReq.m_eHedgeFlag = HEDGE_FLAG_SPECULATION;

	bool insertPortfolio = !pOrderReqDO->PortfolioID().empty();

	int reqId = (int)pOrderReqDO->OrderID;

	if (insertPortfolio)
	{
		OrderReqCache::Insert(reqId, *pOrderReqDO);
	}

	((XTRawAPI*)rawAPI)->get()->order(&orderReq, reqId);

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
	if (rawRespParams.size() > 1)
	{
		auto orderId = *(int*)rawRespParams[1];

		ret = XTUtility::ParseRawOrder(serialId, orderId, (XtError*)rawRespParams[2]);
	}
	else
	{
		ret = XTUtility::ParseRawOrder((COrderError*)rawRespParams[0]);
	}

	return ret;
}