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
	_pricingNotifers(SessionContainer<ContractKey, ContractKeyHash>::NewInstancePtr()),
	_tradingDeskNotifers(SessionContainer<ContractKey, ContractKeyHash>::NewInstancePtr()),
	_otcOrderNotifers(SessionContainer<uint64_t>::NewInstancePtr()),
	_pricingCtx(pricingCtx), _baseContractStrategyMap(1024)
	// _exchangeStrategyMap(1024), _otcStrategySet(1024)
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
		auto& cache = GetInstrumentCache();
		for (auto& contract : *vectPtr)
			cache.Add(contract);

		return vectPtr->size();
	}

	return 0;
}

int OTCWorkerProcessor::LoadStrategyToCache(ProductType productType)
{
	auto allStrategy = StrategyContractDAO::LoadStrategyContractByProductType(productType);
	auto strategyMap = PricingDataContext()->GetStrategyMap();
	auto userStrategyMap = PricingDataContext()->GetUserStrategyMap();
	for (auto& strategy_ptr : *allStrategy)
	{
		if (strategy_ptr->PricingModel)
		{
			// Pricing Model Initialization
			if (auto modelptr = StrategyModelCache::FindOrRetrieveModel(*strategy_ptr->PricingModel))
			{
				strategy_ptr->PricingModel = modelptr;
			}

			if (auto model = ComplexAlgoirthmManager::Instance()->FindModel(strategy_ptr->PricingModel->Model))
			{
				auto& params = model->DefaultParams();
				strategy_ptr->PricingModel->Params.insert(params.begin(), params.end());
				model->ParseParams(strategy_ptr->PricingModel->Params, strategy_ptr->PricingModel->ParsedParams);
			}
		}

		// Implied Volatility Model Initialization
		if (strategy_ptr->IVModel)
		{
			if (auto modelptr = StrategyModelCache::FindOrRetrieveModel(*strategy_ptr->IVModel))
			{
				strategy_ptr->IVModel = modelptr;
			}

			if (auto model = ComplexAlgoirthmManager::Instance()->FindModel(strategy_ptr->IVModel->Model))
			{
				auto& params = model->DefaultParams();
				strategy_ptr->IVModel->Params.insert(params.begin(), params.end());
				model->ParseParams(strategy_ptr->IVModel->Params, strategy_ptr->IVModel->ParsedParams);
			}
		}


		// Volatility Model Initialization
		if (strategy_ptr->VolModel)
		{
			if (auto modelptr = StrategyModelCache::FindOrRetrieveModel(*strategy_ptr->VolModel))
			{
				strategy_ptr->VolModel = modelptr;
			}

			if (auto model = ComplexAlgoirthmManager::Instance()->FindModel(strategy_ptr->VolModel->Model))
			{
				auto& params = model->DefaultParams();
				strategy_ptr->VolModel->Params.insert(params.begin(), params.end());
				model->ParseParams(strategy_ptr->VolModel->Params, strategy_ptr->VolModel->ParsedParams);
			}
		}

		strategyMap->insert(*strategy_ptr, strategy_ptr);
		auto& stSymbMap = userStrategyMap->getorfill(strategy_ptr->UserID());
		auto& stMap_Ptr = stSymbMap.getorfill(strategy_ptr->StrategyName);
		if (!stMap_Ptr)
		{
			stMap_Ptr = std::make_shared<StrategyContractDOMap>(8);
		}
		stMap_Ptr->insert(*strategy_ptr, strategy_ptr);
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
	if (!pMdMap->contains(contractId.InstrumentID()))
	{
		MarketDataDO mdo(contractId.ExchangeID(), contractId.InstrumentID());
		pMdMap->insert(contractId.InstrumentID(), std::move(mdo));
	}
}

int OTCWorkerProcessor::SubscribePricingContracts(const ContractKey& strategyKey, const StrategyPricingContract& strategyContract)
{
	if (!strategyKey.IsOTC())
	{
		AddContractToMonitor(strategyKey);
		if (!_baseContractStrategyMap.contains(strategyKey))
		{
			cuckoohashmap_wrapper<ContractKey, bool, ContractKeyHash> cw(true, 16);
			_baseContractStrategyMap.insert(strategyKey, std::move(cw));
		}

		auto md2stMap = _baseContractStrategyMap.find(strategyKey);
		md2stMap.map()->insert(strategyKey, false);

		// _exchangeStrategyMap.insert(bsContract, strategyKey);
		SubscribeMarketData(strategyKey);
	}

	for (auto& bsContract : strategyContract.PricingContracts)
	{
		if (!bsContract.IsOTC())
		{
			AddContractToMonitor(bsContract);
			if (!_baseContractStrategyMap.contains(bsContract))
			{
				cuckoohashmap_wrapper<ContractKey, bool, ContractKeyHash> cw(true, 16);
				_baseContractStrategyMap.insert(bsContract, std::move(cw));
			}

			auto md2stMap = _baseContractStrategyMap.find(bsContract);
			md2stMap.map()->insert(strategyKey, false);

			// _exchangeStrategyMap.insert(bsContract, strategyKey);
			SubscribeMarketData(bsContract);
		}
	}

	return 0;
}

int OTCWorkerProcessor::UnsubscribePricingContracts(const ContractKey& strategyKey, const StrategyPricingContract& strategyContract)
{
	for (auto& bsContract : strategyContract.PricingContracts)
	{
		if (!bsContract.IsOTC())
		{
			cuckoohashmap_wrapper<ContractKey, bool, ContractKeyHash> strategyMap;
			if (_baseContractStrategyMap.find(bsContract, strategyMap))
				strategyMap.map()->erase(bsContract);
		}
	}

	return 0;
}

int OTCWorkerProcessor::SubscribeStrategy(const StrategyContractDO& strategyDO)
{
	//subscribe market data from CTP
	if (strategyDO.PricingContracts)
		SubscribePricingContracts(strategyDO, *strategyDO.PricingContracts);

	if (strategyDO.IVMContracts)
		SubscribePricingContracts(strategyDO, *strategyDO.IVMContracts);

	if (strategyDO.VolContracts)
		SubscribePricingContracts(strategyDO, *strategyDO.VolContracts);

	return 0;
}

void OTCWorkerProcessor::RegisterPricingListener(const ContractKey& contractId,
	const IMessageSession_Ptr& sessionPtr)
{
	_pricingNotifers->add(contractId, sessionPtr);
}

void OTCWorkerProcessor::UnregisterPricingListener(const ContractKey & contractId, const IMessageSession_Ptr& sessionPtr)
{
	_pricingNotifers->remove(contractId, sessionPtr);
}

void OTCWorkerProcessor::RegisterTradingDeskListener(const ContractKey & contractId, const IMessageSession_Ptr& sessionPtr)
{
	_tradingDeskNotifers->add(contractId, sessionPtr);
}

void OTCWorkerProcessor::UnregisterTradingDeskListener(const ContractKey & contractId, const IMessageSession_Ptr& sessionPtr)
{
	_tradingDeskNotifers->remove(contractId, sessionPtr);
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
	if (strategyDO.PricingModel &&
		(strategyDO.BidEnabled || strategyDO.AskEnabled))
	{
		auto pricingCtx = PricingDataContext();
		if (auto pricingDO = PricingUtility::Pricing(nullptr, strategyDO, *pricingCtx))
		{
			pricingCtx->GetPricingDataDOMap()->upsert(strategyDO, [&pricingDO](IPricingDO_Ptr& pricing_ptr) { pricing_ptr = pricingDO; }, pricingDO);

			if (!strategyDO.BidEnabled)
			{
				pricingDO->Bid().Clear();
			}

			if (!strategyDO.AskEnabled)
			{
				pricingDO->Ask().Clear();
			}

			_pricingNotifers->foreach(strategyDO, [&pricingDO, &strategyDO, &pricingCtx](const IMessageSession_Ptr& session_ptr)
			{if (auto process_ptr = session_ptr->LockMessageProcessor())
				OnResponseProcMacro(process_ptr, MSG_ID_RTN_PRICING, strategyDO.SerialId, &pricingDO, &strategyDO, pricingCtx.get()); });
		}
	}
}

void OTCWorkerProcessor::SendOTCPricing(const StrategyContractDO& strategyDO)
{
	auto pricingCtx = PricingDataContext();
	IPricingDO_Ptr pricingDO;
	if (pricingCtx->GetPricingDataDOMap()->find(strategyDO, pricingDO))
	{
		if (!strategyDO.BidEnabled)
		{
			pricingDO->Bid().Clear();
		}

		if (!strategyDO.AskEnabled)
		{
			pricingDO->Ask().Clear();
		}

		_pricingNotifers->foreach(strategyDO, [&pricingDO, &strategyDO, &pricingCtx](const IMessageSession_Ptr& session_ptr)
		{if (auto process_ptr = session_ptr->LockMessageProcessor())
			OnResponseProcMacro(process_ptr, MSG_ID_RTN_PRICING, strategyDO.SerialId, &pricingDO, &strategyDO, pricingCtx.get()); });
	}
}

void OTCWorkerProcessor::TriggerTadingDeskParams(const StrategyContractDO & strategyDO)
{
	auto pricingCtx = PricingDataContext();
	_tradingDeskNotifers->foreach(strategyDO, [&strategyDO, &pricingCtx](const IMessageSession_Ptr& session_ptr)
	{if (auto process_ptr = session_ptr->LockMessageProcessor())
		OnResponseProcMacro(process_ptr, MSG_ID_RTN_TRADINGDESK_PRICING, strategyDO.SerialId, &strategyDO, pricingCtx.get()); });
}

void OTCWorkerProcessor::TriggerMarketDataUpdating(const MarketDataDO& mdDO)
{
	cuckoohashmap_wrapper<ContractKey, bool, ContractKeyHash> strategyMap;
	if (_baseContractStrategyMap.find(mdDO, strategyMap))
	{
		auto pStrategyMap = PricingDataContext()->GetStrategyMap();
		auto it = strategyMap.map()->lock_table();
		for (auto& pair : it)
		{
			StrategyContractDO_Ptr strategy_ptr;
			if (auto pStrategyDO = pStrategyMap->find(pair.first, strategy_ptr))
			{
				TriggerTadingDeskParams(*strategy_ptr);

				TriggerOTCPricing(*strategy_ptr);

				TriggerOTCTrading(*strategy_ptr);
			}
		}
	}
}


void OTCWorkerProcessor::TriggerOTCTrading(const StrategyContractDO & strategyDO)
{
	GetOTCTradeProcessor()->TriggerHedgeOrderUpdating(strategyDO);

	GetOTCTradeProcessor()->TriggerOTCOrderUpdating(strategyDO);
}

