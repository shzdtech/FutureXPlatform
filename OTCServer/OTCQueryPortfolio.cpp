#include "OTCQueryPortfolio.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../common/Attribute_Key.h"

#include "../message/BizError.h"
#include "../message/MessageUtility.h"

#include "../message/message_macro.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/UserInfoDO.h"

#include "../pricingengine/IPricingDataContext.h"



dataobj_ptr OTCQueryPortfolio::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	if (auto userInfoPtr = std::static_pointer_cast<UserInfoDO>(session->getUserInfo().getExtInfo()))
	{
		ThrowNotFoundExceptionIfEmpty(&userInfoPtr->Portfolios);

		auto ret = std::make_shared<VectorDO<PortfolioDO>>();

		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
		{
			auto pPortfoliorMap = pWorkerProc->PricingDataContext()->GetPortfolioMap();

			auto& userid = session->getUserInfo().getUserId();

			for (auto portfolio : userInfoPtr->Portfolios)
			{
				auto it = pPortfoliorMap->find(PortfolioKey(portfolio, userid));
				if (it != pPortfoliorMap->end())
				{
					ret->push_back(it->second);
				}
			}
		}

		return ret;
	}

	return nullptr;
}
