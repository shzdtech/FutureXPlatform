/***********************************************************************
 * Module:  OTCWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Implementation of the class OTCWorkerProcessor
 ***********************************************************************/

#include "OTCWorkerProcessor.h"
#include "../CTPServer/CTPConstant.h"

#include "../common/Attribute_Key.h"

#include "../message/DefMessageID.h"
#include "../message/SysParam.h"
#include "../message/message_macro.h"
#include "../message/AppContext.h"
#include "../pricingengine/PricingUtility.h"

#include "../databaseop/OTCOrderDAO.h"
#include "../databaseop/ContractDAO.h"

#include "../ordermanager/OrderSeqGen.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCWorkerProcessor::OTCWorkerProcessor(const std::map<std::string, std::string>& configMap)
 // Purpose:    Implementation of OTCWorkerProcessor::OTCWorkerProcessor()
 // Parameters:
 // - frontserver
 // Return:     
 ////////////////////////////////////////////////////////////////////////

OTCWorkerProcessor::OTCWorkerProcessor() :
	_pricingNotifers(SessionContainer<ContractKey>::NewInstance()),
	_otcOrderNotifers(SessionContainer<uint64_t>::NewInstance())
{
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCWorkerProcessor::~OTCWorkerProcessor()
// Purpose:    Implementation of OTCWorkerProcessor::~OTCWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

OTCWorkerProcessor::~OTCWorkerProcessor()
{
	// TODO : implement
}


void OTCWorkerProcessor::AddContractToMonitor(const ContractKey& contractId)
{
	auto mdMap_Ptr = GetOTCTradeProcessor()->GetPricingContext()->GetMarketDataMap();
	if (mdMap_Ptr->find(contractId.InstrumentID()) == mdMap_Ptr->end())
	{
		MarketDataDO mdo(contractId.ExchangeID(), contractId.InstrumentID());
		mdMap_Ptr->emplace(contractId.InstrumentID(), std::move(mdo));
	}
}

int OTCWorkerProcessor::RefreshStrategy(const StrategyContractDO& strategyDO)
{
	int ret = 0;

	//subscribe market data from CTP

	for (auto& bsContract : strategyDO.PricingContracts)
	{
		AddContractToMonitor(bsContract);
		_contract_strategy_map.getorfill(bsContract).insert(strategyDO);
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

void OTCWorkerProcessor::TriggerPricing(const StrategyContractDO& strategyDO)
{
	if (strategyDO.Enabled)
	{
		_pricingNotifers->foreach(strategyDO, [&strategyDO](IMessageSession* pSession)
		{OnResponseProcMacro(pSession->getProcessor(),
			MSG_ID_RTN_PRICING, strategyDO.SerialId, &strategyDO); });
	}
}

void OTCWorkerProcessor::TriggerUpdating(const MarketDataDO& mdDO)
{
	auto strategyMap = GetOTCTradeProcessor()->GetPricingContext()->GetStrategyMap();
	auto it = _contract_strategy_map.find(mdDO);
	if (it != _contract_strategy_map.end())
	{
		for (auto contractID : it->second)
		{
			auto it = strategyMap->find(contractID);
			if (it != strategyMap->end())
			{
				StrategyContractDO& sDO = it->second;
				GetOTCTradeProcessor()->TriggerHedgeOrderUpdating(sDO);
				GetOTCTradeProcessor()->TriggerOTCOrderUpdating(sDO);
				TriggerPricing(sDO);
			}
		}
	}
}

