/***********************************************************************
 * Module:  CTPOCTUnSubMarketData.cpp
 * Author:  milk
 * Modified: 2016年4月19日 12:59:04
 * Purpose: Implementation of the class CTPOCTUnSubMarketData
 ***********************************************************************/

#include "OTCUnSubMarketData.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/FieldName.h"
#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../bizutility/ContractCache.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOCTUnSubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPOCTUnSubMarketData::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCUnSubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto ret = std::make_shared<StringTableDO>();

	auto stdo = (StringTableDO*)reqDO.get();

	if (!stdo->Data.empty())
	{
		auto& instList = stdo->Data.begin()->second;
		auto nInstrument = instList.size();

		if (nInstrument > 0)
			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(session->getProcessor()))
			{
				for (auto& inst : instList)
				{
					if (auto contract = pWorkerProc->GetInstrumentCache().QueryInstrumentById(inst))
					{
						pWorkerProc->UnregisterPricingListener(*contract,
							session->getProcessor()->LockMessageSession().get());

						ret->Data[STR_INSTRUMENT_ID].push_back(inst);
					}
				}

			}
	}

	return ret;
}