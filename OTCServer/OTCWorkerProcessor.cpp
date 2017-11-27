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

#include "../utility/scopeunlock.h"

#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../systemsettings/AppContext.h"
#include "../pricingengine/PricingUtility.h"

#include "../databaseop/OTCOrderDAO.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/PortfolioDAO.h"
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
	_tradingDeskNotifers(SessionContainer<UserContractKey, UserContractKeyHash>::NewInstancePtr()),
	_otcOrderNotifers(SessionContainer<uint64_t>::NewInstancePtr()),
	_pricingCtx(pricingCtx), _baseContractStrategyMap(128)
	// _exchangeStrategyMap(1024), _otcStrategySet(1024)
{
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCWorkerProcessor::~OTCWorkerProcessor()
// Purpose:    Implementation of OTCWorkerProcessor::~OTCWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

OTCWorkerProcessor::~OTCWorkerProcessor()
{
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

int OTCWorkerProcessor::LoadStrategyToCache(const std::string& userId)
{
	StrategyModelCache::Load(userId);

	auto allStrategy = StrategyContractDAO::LoadStrategyContractByProductType(userId);
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

void OTCWorkerProcessor::LoadPortfolios(const std::string& userId)
{
	if (auto vect_portfolio = PortfolioDAO::FindPortfolios(userId))
	{
		auto portfolioMap = PricingDataContext()->GetPortfolioMap();

		for (auto& portfolio : *vect_portfolio)
		{
			portfolioMap->getorfill(portfolio.UserID()).emplace(portfolio.PortfolioID(), portfolio);
		}
	}
}

void OTCWorkerProcessor::Initialize()
{
	LoadContractToCache(GetContractProductType());
	/*LoadPortfolios();
	for (auto productType : GetStrategyProductTypes())
		LoadStrategyToCache(productType);*/
}


void OTCWorkerProcessor::AddContractToMonitor(const ContractKey& contractId)
{
	auto pMdMap = PricingDataContext()->GetMarketDataMap();
	MarketDataDO mdo(contractId.ExchangeID(), contractId.InstrumentID());
	pMdMap->insert(contractId.InstrumentID(), std::move(mdo));
	SubscribeMarketData(contractId);
}

int OTCWorkerProcessor::SubscribePricingContracts(const UserContractKey& strategyKey, const StrategyPricingContract& strategyContract)
{
	for (auto& bsContract : strategyContract.PricingContracts)
	{
		if (!bsContract.IsOTC())
		{
			AddContractToMonitor(bsContract);
			AddMarketDataStrategyTrigger(bsContract, strategyKey);
		}
	}

	return 0;
}

void OTCWorkerProcessor::AddMarketDataStrategyTrigger(const ContractKey& marketContract, const UserContractKey& strategyKey)
{
	if (!_baseContractStrategyMap.contains(marketContract))
	{
		_baseContractStrategyMap.insert(marketContract, std::make_shared<lockfree_set<UserContractKey, UserContractKeyHash>>());
	}

	std::shared_ptr<lockfree_set<UserContractKey, UserContractKeyHash>> md2stSet;
	if (_baseContractStrategyMap.find(marketContract, md2stSet))
		md2stSet->emplace(strategyKey);
}


int OTCWorkerProcessor::UnsubscribePricingContracts(const UserContractKey& strategyKey, const StrategyPricingContract& strategyContract)
{
	for (auto& bsContract : strategyContract.PricingContracts)
	{
		if (!bsContract.IsOTC())
		{
			std::shared_ptr<lockfree_set<UserContractKey, UserContractKeyHash>> md2stSet;
			if (_baseContractStrategyMap.find(bsContract, md2stSet))
				md2stSet->erase(strategyKey);
		}
	}

	return 0;
}

int OTCWorkerProcessor::SubscribeStrategy(const StrategyContractDO& strategyDO)
{
	if (!strategyDO.IsOTC())
	{
		AddMarketDataStrategyTrigger(strategyDO, strategyDO);
		AddContractToMonitor(strategyDO);
	}

	if (strategyDO.BaseContract)
	{
		if (!strategyDO.BaseContract->IsOTC())
		{
			AddContractToMonitor(*strategyDO.BaseContract);
		}
	}

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

void OTCWorkerProcessor::RegisterTradingDeskListener(const UserContractKey & userContractId, const IMessageSession_Ptr& sessionPtr)
{
	_tradingDeskNotifers->add(userContractId, sessionPtr);
}

void OTCWorkerProcessor::UnregisterTradingDeskListener(const UserContractKey & userContractId, const IMessageSession_Ptr& sessionPtr)
{
	_tradingDeskNotifers->remove(userContractId, sessionPtr);
}

InstrumentCache & OTCWorkerProcessor::GetInstrumentCache()
{
	return ContractCache::Get(ProductCacheType::PRODUCT_CACHE_OTC_CONTRACT);
}

const std::set<ProductType>& OTCWorkerProcessor::GetStrategyProductTypes() const
{
	return _strategyProductTypes;
}

IPricingDataContext_Ptr& OTCWorkerProcessor::PricingDataContext()
{
	return _pricingCtx;
}

void OTCWorkerProcessor::TriggerOTCPricing(const StrategyContractDO& strategyDO, bool findInCache)
{
	auto& pricingCtx = PricingDataContext();

	IPricingDO_Ptr pricingDO;
	if (!pricingCtx->GetPricingDataDOMap()->find(strategyDO, pricingDO) || !findInCache)
	{
		if (strategyDO.PricingModel)
		{
			try
			{
				if (pricingDO = PricingUtility::Pricing(nullptr, strategyDO, pricingCtx))
				{
					pricingCtx->GetPricingDataDOMap()->upsert(strategyDO,
						[&pricingDO](IPricingDO_Ptr& pricing_ptr) { pricing_ptr = pricingDO; },
						pricingDO);
				}
			}
			catch (std::exception& e)
			{
				LOG_ERROR << e.what();
			}
		}
	}

	if (pricingDO && (strategyDO.BidEnabled || strategyDO.AskEnabled))
	{
		auto pricingCopy = std::make_shared<PricingDO>(pricingDO->ExchangeID(), pricingDO->InstrumentID());

		if (strategyDO.BidEnabled)
		{
			pricingCopy->Bid() = pricingDO->Bid();
		}

		if (strategyDO.AskEnabled)
		{
			pricingCopy->Ask() = pricingDO->Ask();
		}

		_pricingNotifers->foreach(strategyDO, [&pricingCopy, &strategyDO, &pricingCtx](const IMessageSession_Ptr& session_ptr)
		{if (auto process_ptr = session_ptr->LockMessageProcessor())
			OnResponseProcMacro(process_ptr, MSG_ID_RTN_PRICING, strategyDO.SerialId, &pricingCopy); });
	}
}

bool OTCWorkerProcessor::TriggerTradingDeskParams(const StrategyContractDO & strategyDO)
{
	return false;
}

void OTCWorkerProcessor::TriggerUpdateByMarketData(const MarketDataDO& mdDO)
{
	std::shared_ptr<lockfree_set<UserContractKey, UserContractKeyHash>> md2stSet;
	if (_baseContractStrategyMap.find(mdDO, md2stSet))
	{
		auto pStrategyMap = PricingDataContext()->GetStrategyMap();

		scopeunlock<lockfree_set<UserContractKey, UserContractKeyHash>> sc(md2stSet.get());
		for (auto& key : md2stSet->rawset())
		{
			StrategyContractDO_Ptr strategy_ptr;
			if (auto pStrategyDO = pStrategyMap->find(key, strategy_ptr))
			{
				TriggerPricingByStrategy(*strategy_ptr);
			}
		}
		for (auto& key : md2stSet->rawset())
		{
			StrategyContractDO_Ptr strategy_ptr;
			if (auto pStrategyDO = pStrategyMap->find(key, strategy_ptr))
			{
				TriggerOTCTradingByStrategy(*strategy_ptr);
			}
		}
	}
}

void OTCWorkerProcessor::TriggerOTCTradingByStrategy(const StrategyContractDO & strategyDO)
{
	GetOTCTradeWorkerProcessor()->TriggerOTCOrderUpdating(strategyDO);

	GetOTCTradeWorkerProcessor()->TriggerAutoOrderUpdating(strategyDO);

	GetOTCTradeWorkerProcessor()->TriggerHedgeOrderUpdating(strategyDO);
}

void OTCWorkerProcessor::TriggerPricingByStrategy(const StrategyContractDO & strategyDO)
{
	bool cached = TriggerTradingDeskParams(strategyDO);

	TriggerOTCPricing(strategyDO, cached);
}



