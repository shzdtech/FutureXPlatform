/***********************************************************************
 * Module:  OTCWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Implementation of the class OTCWorkerProcessor
 ***********************************************************************/

#include "OTCWorkerProcessor.h"
#include "../litelogger/LiteLogger.h"

#include "../CTPServer/CTPConstant.h"

#include "../common/Attribute_Key.h"

#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../systemsettings/AppContext.h"
#include "../pricingengine/PricingUtility.h"

#include "../databaseop/OTCOrderDAO.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"
#include "../databaseop/ModelParamsDAO.h"
#include "../pricingengine/ComplexAlgoirthmManager.h"

#include "../bizutility/ContractCache.h"
#include "../bizutility/StrategyModelCache.h"

#include "../ordermanager/OrderSeqGen.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCWorkerProcessor::OTCWorkerProcessor(const std::map<std::string, std::string>& configMap)
 // Purpose:    Implementation of OTCWorkerProcessor::OTCWorkerProcessor()
 // Parameters:
 // - frontserver
 // Return:     
 ////////////////////////////////////////////////////////////////////////

OTCWorkerProcessor::OTCWorkerProcessor(const IPricingDataContext_Ptr& pricingCtx) :
	_pricingNotifers(SessionContainer<ContractKey>::NewInstancePtr()),
	_tradingDeskNotifers(SessionContainer<ContractKey>::NewInstancePtr()),
	_otcOrderNotifers(SessionContainer<uint64_t>::NewInstancePtr()),
	_pricingCtx(pricingCtx)
{
	_runingTradingDeskFlag = true;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCWorkerProcessor::~OTCWorkerProcessor()
// Purpose:    Implementation of OTCWorkerProcessor::~OTCWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

OTCWorkerProcessor::~OTCWorkerProcessor()
{
	_runingTradingDeskFlag = false;
}


int OTCWorkerProcessor::LoadContractToCache(ProductType productType)
{
	if (auto vectPtr = ContractDAO::FindContractByProductType(productType))
	{
		for (auto& contract : *vectPtr)
			GetInstrumentCache().Add(contract);

		return vectPtr->size();
	}

	return 0;
}

int OTCWorkerProcessor::LoadStrategyToCache(ProductType productType)
{
	auto allStrategy = StrategyContractDAO::LoadStrategyContractByProductType(productType);
	auto strategyMap = PricingDataContext()->GetStrategyMap();
	for (auto& strategy : *allStrategy)
	{
		if (strategy.PricingModel)
		{
			// Pricing Model Initialization
			if (auto modelptr = StrategyModelCache::FindOrCreateModel(*strategy.PricingModel))
			{
				strategy.PricingModel = modelptr;
			}

			if (auto model = ComplexAlgoirthmManager::Instance()->FindModel(strategy.PricingModel->Model))
			{
				auto& params = model->DefaultParams();
				strategy.PricingModel->Params.insert(params.begin(), params.end());
				model->ParseParams(strategy.PricingModel->Params, strategy.PricingModel->ParsedParams);
			}
		}

		// Implied Volatility Model Initialization
		if (strategy.IVModel)
		{
			if (auto modelptr = StrategyModelCache::FindOrCreateModel(*strategy.IVModel))
			{
				strategy.IVModel = modelptr;
			}

			if (auto model = ComplexAlgoirthmManager::Instance()->FindModel(strategy.IVModel->Model))
			{
				auto& params = model->DefaultParams();
				strategy.IVModel->Params.insert(params.begin(), params.end());
				model->ParseParams(strategy.IVModel->Params, strategy.IVModel->ParsedParams);
			}
		}


		// Volatility Model Initialization
		if (strategy.VolModel)
		{
			if (auto modelptr = StrategyModelCache::FindOrCreateModel(*strategy.VolModel))
			{
				strategy.VolModel = modelptr;
			}

			if (auto model = ComplexAlgoirthmManager::Instance()->FindModel(strategy.VolModel->Model))
			{
				auto& params = model->DefaultParams();
				strategy.VolModel->Params.insert(params.begin(), params.end());
				model->ParseParams(strategy.VolModel->Params, strategy.VolModel->ParsedParams);
			}
		}

		strategyMap->emplace(strategy, strategy);
	}

	return allStrategy->size();
}


void OTCWorkerProcessor::Initialize()
{
	LoadContractToCache(GetContractProductType());
	for (auto productType : GetStrategyProductTypes())
		LoadStrategyToCache(productType);
}


void OTCWorkerProcessor::AddContractToMonitor(const ContractKey& contractId)
{
	auto pMdMap = PricingDataContext()->GetMarketDataMap();
	if (!pMdMap->tryfind(contractId.InstrumentID()))
	{
		MarketDataDO mdo(contractId.ExchangeID(), contractId.InstrumentID());
		pMdMap->emplace(contractId.InstrumentID(), std::move(mdo));
	}
}

int OTCWorkerProcessor::SubscribeStrategy(const StrategyContractDO& strategyDO)
{
	//subscribe market data from CTP

	if (strategyDO.IsOTC())
	{
		_otcStrategySet.emplace(strategyDO);
		for (auto& bsContract : strategyDO.PricingContracts)
		{
			if (!bsContract.IsOTC())
			{
				AddContractToMonitor(bsContract);
				_baseContractStrategyMap.getorfill(bsContract).insert(strategyDO);
				SubscribeMarketData(bsContract);
			}
		}
	}
	else
	{
		AddContractToMonitor(strategyDO);
		_exchangeStrategySet.emplace(strategyDO);
		SubscribeMarketData(strategyDO);
		for (auto& bsContract : strategyDO.PricingContracts)
		{
			if (!bsContract.IsOTC())
			{
				AddContractToMonitor(bsContract);
				SubscribeMarketData(bsContract);
			}
		}
	}

	return strategyDO.PricingContracts.size();
}

void OTCWorkerProcessor::RegisterPricingListener(const ContractKey& contractId,
	IMessageSession* pMsgSession)
{
	_pricingNotifers->add(contractId, pMsgSession);
}

void OTCWorkerProcessor::UnregisterPricingListener(const ContractKey & contractId, IMessageSession * pMsgSession)
{
	_pricingNotifers->remove(contractId, pMsgSession);
}

void OTCWorkerProcessor::RegisterTradingDeskListener(const ContractKey & contractId, IMessageSession * pMsgSession)
{
	_tradingDeskNotifers->add(contractId, pMsgSession);
}

void OTCWorkerProcessor::UnregisterTradingDeskListener(const ContractKey & contractId, IMessageSession * pMsgSession)
{
	_tradingDeskNotifers->remove(contractId, pMsgSession);
}

InstrumentCache & OTCWorkerProcessor::GetInstrumentCache()
{
	static InstrumentCache& cache = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_OTC_CONTRACT);
	return cache;
}

IPricingDataContext_Ptr& OTCWorkerProcessor::PricingDataContext()
{
	return _pricingCtx;
}

void OTCWorkerProcessor::TriggerOTCPricing(const StrategyContractDO& strategyDO)
{
	if (strategyDO.BidEnabled || strategyDO.AskEnabled)
	{
		auto pricingCtx = PricingDataContext();
		_pricingNotifers->foreach(strategyDO, [&strategyDO, &pricingCtx](IMessageSession* pSession)
		{OnResponseProcMacro(pSession->getProcessor(),
			MSG_ID_RTN_PRICING, strategyDO.SerialId, &strategyDO, pricingCtx.get()); });
	}
}

void OTCWorkerProcessor::TriggerTadingDeskParams(const StrategyContractDO & strategyDO)
{
	auto pricingCtx = PricingDataContext();
	_tradingDeskNotifers->foreach(strategyDO, [&strategyDO, &pricingCtx](IMessageSession* pSession)
	{OnResponseProcMacro(pSession->getProcessor(),
		MSG_ID_RTN_TRADINGDESK_PRICING, strategyDO.SerialId, &strategyDO, pricingCtx.get()); });
}

void OTCWorkerProcessor::TriggerUpdating(const MarketDataDO& mdDO)
{
	auto strategyMap = PricingDataContext()->GetStrategyMap();
	if (auto pStrategy = _baseContractStrategyMap.tryfind(mdDO))
	{
		for (auto contractID : *pStrategy)
		{
			if (auto pStrategyDO = strategyMap->tryfind(contractID))
			{
				TriggerOTCUpdating(*pStrategyDO);
			}
		}
	}
}

void OTCWorkerProcessor::TriggerOTCUpdating(const StrategyContractDO & strategyDO)
{
	GetOTCTradeProcessor()->TriggerHedgeOrderUpdating(strategyDO);
	GetOTCTradeProcessor()->TriggerOTCOrderUpdating(strategyDO);
	TriggerOTCPricing(strategyDO);
}

