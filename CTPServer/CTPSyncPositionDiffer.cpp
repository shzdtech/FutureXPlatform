/***********************************************************************
 * Module:  CTPSyncPositionDiffer.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:38:54
 * Purpose: Implementation of the class CTPSyncPositionDiffer
 ***********************************************************************/

#include "CTPSyncPositionDiffer.h"
#include "CTPRawAPI.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/ResultDO.h"
#include "../message/BizError.h"
#include "../message/MessageUtility.h"
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"
#include "../databaseop/PositionDAO.h"

#include "CTPUtility.h"
#include "CTPTradeWorkerProcessor.h"

#include "../bizutility/ContractCache.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPSyncPositionDiffer::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPSyncPositionDiffer::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPSyncPositionDiffer::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto pPositionMap = (MapDO<std::tuple<std::string, std::string, PositionDirectionType>, std::pair<int, int>>*)reqDO.get();

	auto& userInfo = session->getUserInfo();
	auto role = userInfo.getRole();

	if (role >= ROLE_TRADINGDESK)
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessorBase>(msgProcessor))
		{
			std::vector<UserPositionDO> userPosition;

			for (auto pair : *pPositionMap)
			{
				std::string contract, portfolio;
				PositionDirectionType direction;
				std::tie(contract, portfolio, direction) = pair.first;

				std::string exchange;
				int multiplier = 1;
				if (auto pInstrumentDO = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(contract))
				{
					exchange = pInstrumentDO->ExchangeID();
					multiplier = pInstrumentDO->VolumeMultiple;
				}

				auto position_ptr = std::make_shared<UserPositionExDO>(exchange, contract, portfolio, userInfo.getUserId());
				position_ptr->Direction = direction;
				position_ptr->YdInitPosition = pair.second.second;
				position_ptr->TradingDay = userInfo.getTradingDay();
				position_ptr->Multiplier = multiplier;

				if (auto pMktDataMap = pWorkerProc->GetMarketDataDOMap())
				{
					MarketDataDO mdo;
					if (pMktDataMap->find(contract, mdo))
					{
						position_ptr->LastPrice = mdo.LastPrice;
						position_ptr->PreSettlementPrice = mdo.PreSettlementPrice;
					}
				}

				userPosition.push_back(*position_ptr);
				ManualOpHub::Instance()->NotifyUpdateManualPosition(*position_ptr);
			}

			if (pWorkerProc->IsLoadPositionFromDB() && !userPosition.empty())
			{
				PositionDAO::SyncPosition(userInfo.getUserId(), userInfo.getInvestorId(), userPosition);
			}
		}
	}

	return std::make_shared<ResultDO>();
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPSyncPositionDiffer::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPSyncPositionDiffer::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPSyncPositionDiffer::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}