#include "CTPOTCQueryRisk.h"

#include "CTPOTCWorkerProcessor.h"

#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"

#include "../message/BizError.h"
#include "../message/message_macro.h"
#include "../message/IMessageServiceLocator.h"

#include "../dataobject/TemplateDO.h"

dataobj_ptr CTPOTCQueryRisk::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr & msgProcessor, const IMessageSession_Ptr & session)
{
	CheckLogin(session);

	auto stdo = (StringMapDO<std::string>*)reqDO.get();

	auto ret = std::make_shared<VectorDO<RiskDO>>();

	if (!stdo->empty())
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCWorkerProcessor>(msgProcessor))
		{
			UnderlyingRiskMap riskMap;

			auto pOTCTradeProc = (CTPOTCTradeProcessor*)pWorkerProc->GetOTCTradeProcessor();

			pOTCTradeProc->GetUserPositionContext()->GetRiskByPortfolio(session->getUserInfo().getUserId(), stdo->begin()->second, riskMap);
			
			pOTCTradeProc->GetOTCOrderManager().GetPositionContext().GetRiskByPortfolio(session->getUserInfo().getUserId(), stdo->begin()->second, riskMap);

			for (auto it : riskMap)
			{
				for (auto rit : it.second)
				{
					ret->push_back(std::move(rit.second));
				}
			}
		}
	}

	return ret;
}
