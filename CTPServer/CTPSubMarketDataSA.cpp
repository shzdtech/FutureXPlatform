/***********************************************************************
 * Module:  CTPSubMarketDataSA.cpp
 * Author:  milk
 * Modified: 2015年3月6日 20:11:00
 * Purpose: Implementation of the class CTPSubMarketDataSA
 ***********************************************************************/

#include "CTPSubMarketDataSA.h"
#include "CTPRawAPI.h"
#include "CTPMarketDataSAProcessor.h"
#include "../databaseop/ContractDAO.h"
#include "../message/MessageUtility.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../utility/TUtil.h"
#include "../bizutility/ContractCache.h"
#include "../common/BizErrorIDs.h"

#include <algorithm>
#include "CTPUtility.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPSubMarketDataSA::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPSubMarketDataSA::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPSubMarketDataSA::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto stdo = (StringTableDO*)reqDO.get();
	auto ret = std::make_shared<VectorDO<MarketDataDO>>();

	if (!stdo->Data.empty())
	{
		auto& instList = stdo->Data.begin()->second;
		auto nInstrument = instList.size();
		if (nInstrument > 0)
		{
			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPMarketDataSAProcessor>(msgProcessor))
			{
				std::vector<std::string> subList;
				int tradingDay = pWorkerProc->GetSystemUser().getTradingDay();
				auto& mdMap = pWorkerProc->GetMarketDataMap();
				for (auto& inst : instList)
				{
					MarketDataDO mdo;
					if (mdMap.find(inst, mdo))
					{
						ret->push_back(std::move(mdo));
						pWorkerProc->MarketDataNotifers->add(inst, session);
					}
					else
					{
						InstrumentDO* pInstumentDO = nullptr;
						if (pInstumentDO = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(inst))
						{
							MarketDataDO mdo(pInstumentDO->ExchangeID(), pInstumentDO->InstrumentID());
							mdo.TradingDay = tradingDay;
							ret->push_back(mdo);
							mdMap.insert(inst, mdo);

							subList.push_back(inst);
							pWorkerProc->MarketDataNotifers->add(inst, session);
						}
					}
				}

				if (auto nSubs = subList.size())
				{
					std::unique_ptr<char*[]> ppInstrments(new char*[nSubs]);

					for (int i = 0; i < nSubs; i++)
					{
						ppInstrments[i] = const_cast<char*>(subList[i].data());
					}

					if (auto mdApiProxy = ((CTPRawAPI*)rawAPI)->MdAPIProxy()->get())
					{
						mdApiProxy->SubscribeMarketData(ppInstrments.get(), nSubs);
					}
				}
			}
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPSubMarketDataSA::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPSubMarketDataSA::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPSubMarketDataSA::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	/*CTPUtility::CheckError(rawRespParams[1]);

	VectorDO_Ptr<MarketDataDO> ret = std::make_shared<VectorDO<MarketDataDO>>();

	ret->HasMore = !*(bool*)rawRespParams[3];

	if (auto pRspInstr = (CThostFtdcSpecificInstrumentField*)rawRespParams[0])
	{
		std::string exchange;
		if (auto pInstumentDO = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(pRspInstr->InstrumentID))
		{
			exchange = pInstumentDO->ExchangeID();
		}
		else
		{
			std::string errmsg("Contract code: ");
			throw NotFoundException(errmsg + pRspInstr->InstrumentID + " does not exists.");
		}

		ret->push_back(MarketDataDO(exchange, pRspInstr->InstrumentID));
	}*/

	return nullptr;
}