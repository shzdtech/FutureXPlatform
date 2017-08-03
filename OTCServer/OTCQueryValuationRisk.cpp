#include "OTCQueryValuationRisk.h"

#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"

#include "../message/BizError.h"
#include "../message/message_macro.h"
#include "../message/IMessageServiceLocator.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/ValuationRiskDO.h"

dataobj_ptr OTCQueryValuationRisk::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr & msgProcessor, const IMessageSession_Ptr & session)
{
	CheckLogin(session);
	CheckRolePermission(session, UserRoleType::ROLE_TRADINGDESK);

	auto pValuationDO = (ValuationRiskDO*)reqDO.get();

	auto ret = std::make_shared<VectorDO<RiskDO>>();

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		UnderlyingRiskMap riskMap;
		pWorkerProc->GetOTCTradeProcessor()->GetOTCOrderManager().
			GetPositionContext().GetValuationRiskByPortfolio(pWorkerProc->PricingDataContext(), session->getUserInfo().getUserId(), *pValuationDO, riskMap);

		for (auto it : riskMap)
		{
			for (auto rit : it.second)
			{
				ret->push_back(std::move(rit.second));
			}
		}
	}

	return ret;
}
