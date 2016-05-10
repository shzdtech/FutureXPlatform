/***********************************************************************
 * Module:  CTPOTCWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Implementation of the class CTPOTCWorkerProcessor
 ***********************************************************************/

#include "CTPOTCWorkerProcessor.h"
#include "../CTPServer/CTPConstant.h"
#include "../CTPServer/CTPUtility.h"
#include <glog/logging.h>

#include "../message/DefMessageID.h"
#include "../message/SysParam.h"
#include "../message/message_marco.h"
#include "../pricingengine/PricingUtility.h"

#include "../databaseop/OTCOrderDAO.h"
#include "../databaseop/ContractDAO.h"

#include "../pricingengine/PricingContext.h"
#include "../ordermanager/OrderSeqGen.h"
////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCWorkerProcessor::CTPOTCWorkerProcessor(const std::map<std::string, std::string>& configMap)
// Purpose:    Implementation of CTPOTCWorkerProcessor::CTPOTCWorkerProcessor()
// Parameters:
// - frontserver
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCWorkerProcessor::CTPOTCWorkerProcessor(const std::map<std::string, std::string>& configMap)
	: CTPMarketDataProcessor(configMap),
	_otcTradeProcessor(configMap),
	_pricingNotifers(new SessionContainer<ContractKey>()),
	_otcOrderNotifers(new SessionContainer<uint64_t>())
{
	_isLogged = false;
	_rawAPI.TrdAPI = ((CTPRawAPI*)(_otcTradeProcessor.getRawAPI()))->TrdAPI;
	auto pContractMap = PricingContext::Instance()->GetContractMap();
	if (pContractMap->size() == 0)
	{
		*pContractMap = *ContractDAO::FindAllContract();
	}
	LoginIfNeed();
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCWorkerProcessor::~CTPOTCWorkerProcessor()
// Purpose:    Implementation of CTPOTCWorkerProcessor::~CTPOTCWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCWorkerProcessor::~CTPOTCWorkerProcessor()
{
	// TODO : implement
}

bool CTPOTCWorkerProcessor::OnSessionClosing(void)
{
	return _otcTradeProcessor.OnSessionClosing();
}

void CTPOTCWorkerProcessor::setSession(IMessageSession* msgSession)
{
	CTPMarketDataProcessor::setSession(msgSession);
	_otcTradeProcessor.setSession(msgSession);
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCWorkerProcessor::LoginIfNeed()
// Purpose:    Implementation of CTPOTCWorkerProcessor::LoginIfNeed()
// Return:     void
////////////////////////////////////////////////////////////////////////

int CTPOTCWorkerProcessor::LoginIfNeed(void)
{
	int ret = 0;

	if (!_isLogged)
	{
		CThostFtdcReqUserLoginField req;
		std::memset(&req, 0, sizeof(req));
		std::strcpy(req.BrokerID, SysParam::Get(CTP_MD_BROKERID).data());
		std::strcpy(req.UserID, SysParam::Get(CTP_MD_USERID).data());
		std::strcpy(req.Password, SysParam::Get(CTP_MD_PASSWORD).data());
		ret = ((CTPRawAPI*)getRawAPI())->MdAPI->ReqUserLogin(&req, AppContext::GenNextSeq());
	}

	_otcTradeProcessor.LoginIfNeed();

	return ret;
}

void CTPOTCWorkerProcessor::AddContractToMonitor(const ContractKey& contractId)
{
	auto mdMap_Ptr = PricingContext::Instance()->GetMarketDataDOMap();
	if (mdMap_Ptr->find(contractId.InstrumentID()) == mdMap_Ptr->end())
	{
		MarketDataDO mdo(contractId.ExchangeID(), contractId.InstrumentID());
		mdMap_Ptr->emplace(contractId.InstrumentID(), std::move(mdo));
	}
}

int CTPOTCWorkerProcessor::RefreshStrategy(const StrategyContractDO& strategyDO)
{
	int ret = 0;

	//subscribe market data from CTP
	auto baseContracts = strategyDO.BaseContracts;
	int size = baseContracts->size();
	int i = 0;
	if (!strategyDO.IsOTC())
		size++;
	auto instPtr = std::make_unique<char*[]>(size);
	if (!strategyDO.IsOTC())
	{
		AddContractToMonitor(strategyDO);
		instPtr[i++] = const_cast<char*>(strategyDO.InstrumentID().data());
	}

	for (auto& bsContract : *baseContracts)
	{
		AddContractToMonitor(bsContract);

		_contract_strategy_map.getorfill(bsContract).insert(strategyDO);

		instPtr[i++] = const_cast<char*>(bsContract.InstrumentID().data());
	}
	ret = _rawAPI.MdAPI->SubscribeMarketData(instPtr.get(), i);

	return ret;
}

void CTPOTCWorkerProcessor::RegisterPricingListener(const ContractKey& contractId,
	IMessageSession* pMsgSession)
{
	_pricingNotifers->add(contractId, pMsgSession);
}

void CTPOTCWorkerProcessor::UnRegisterPricingListener(const ContractKey & contractId, IMessageSession * pMsgSession)
{
	_pricingNotifers->remove(contractId, pMsgSession);
}

void CTPOTCWorkerProcessor::RegisterOTCOrderListener(const uint64_t orderID,
	IMessageSession* pMsgSession)
{
	_otcOrderNotifers->add(orderID, pMsgSession);
}

void CTPOTCWorkerProcessor::RegisterOrderListener(const uint64_t orderID,
	IMessageSession* pMsgSession)
{
	_otcTradeProcessor.RegisterOrderListener(orderID, pMsgSession);
}


void CTPOTCWorkerProcessor::TriggerPricing(const StrategyContractDO& strategyDO)
{
	if (strategyDO.Enabled)
	{
		ScopeLockContext sclcctx;
		if (auto pNotiferSet = _pricingNotifers->getwithlock(strategyDO, sclcctx))
		{
			for (auto pSession : *pNotiferSet)
			{
				OnResponseProcMarco(pSession->getProcessor(), MSG_ID_RTN_PRICING,
					&strategyDO);
			}
		}
	}
}

void CTPOTCWorkerProcessor::TriggerUpdating(const MarketDataDO& mdDO)
{
	auto strategyMap = PricingContext::Instance()->GetStrategyMap();
	auto it = _contract_strategy_map.find(mdDO);
	if (it != _contract_strategy_map.end())
	{
		for (auto contractID : it->second)
		{
			auto it = strategyMap->find(contractID);
			if (it != strategyMap->end())
			{
				StrategyContractDO& sDO = it->second;
				TriggerOrderUpdating(sDO);
				TriggerOTCOrderUpdating(sDO);
				TriggerPricing(sDO);
			}
		}
	}
}

void CTPOTCWorkerProcessor::TriggerOTCOrderUpdating(const StrategyContractDO& strategyDO)
{
	if (auto updatedOrders = _otcTradeProcessor.GetOTCOrderManager().UpdateOrderByStrategy(strategyDO))
	{
		for (auto& order : *updatedOrders)
		{
			ScopeLockContext sclcctx;
			if (auto pNotiferSet = _otcOrderNotifers->getwithlock(order.OrderID, sclcctx))
			{
				for (auto pSession : *pNotiferSet)
				{
					OnResponseProcMarco(pSession->getProcessor(), MSG_ID_ORDER_UPDATE, &order);
				}
			}
		}
	}
}

void CTPOTCWorkerProcessor::TriggerOrderUpdating(const StrategyContractDO& strategyDO)
{
	_otcTradeProcessor.TriggerOrderUpdating(strategyDO);
}

int CTPOTCWorkerProcessor::OTCNewOrder(OrderDO& orderReq)
{
	return _otcTradeProcessor.GetOTCOrderManager().CreateOrder(orderReq);
}

int CTPOTCWorkerProcessor::OTCCancelOrder(OrderDO& orderReq)
{
	return _otcTradeProcessor.GetOTCOrderManager().CancelOrder(orderReq);
}

void CTPOTCWorkerProcessor::CancelAutoOrder(const UserContractKey& userContractKey)
{
	_otcTradeProcessor.GetAutoOrderManager().CancelOrder(OrderDO(userContractKey));
}



//CTP APIs

void CTPOTCWorkerProcessor::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	auto mdMap = PricingContext::Instance()->GetMarketDataDOMap();
	auto it = mdMap->find(pDepthMarketData->InstrumentID);
	if (it != mdMap->end())
	{
		auto& mdo = it->second;
		if (mdo.BidPrice != pDepthMarketData->BidPrice1 ||
			mdo.AskPrice != pDepthMarketData->AskPrice1 ||
			mdo.BidVolume != pDepthMarketData->BidVolume1 ||
			mdo.AskVolume != pDepthMarketData->AskVolume1)
		{
			mdo.BidPrice = pDepthMarketData->BidPrice1;
			mdo.AskPrice = pDepthMarketData->AskPrice1;
			mdo.BidVolume = pDepthMarketData->BidVolume1;
			mdo.AskVolume = pDepthMarketData->AskVolume1;
			//mdo.PreClosePrice = pDepthMarketData->PreClosePrice;
			//mdo.OpenPrice = pDepthMarketData->OpenPrice;
			//mdo.HighestPrice = pDepthMarketData->HighestPrice;
			//mdo.LowestPrice = pDepthMarketData->LowestPrice;
			//mdo.LastPrice = pDepthMarketData->LastPrice;
			//mdo.Volume = pDepthMarketData->Volume;
			//mdo.Turnover = pDepthMarketData->Turnover;
			//mdo.UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
			//mdo.LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
			//mdo.PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
			//mdo.SettlementPrice = pDepthMarketData->SettlementPrice;

			// Start to trigger pricing
			TriggerUpdating(mdo);
		}
	}
}

void CTPOTCWorkerProcessor::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}

void CTPOTCWorkerProcessor::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}


