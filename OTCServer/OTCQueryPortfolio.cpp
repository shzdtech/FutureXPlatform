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
	CheckRolePermission(session, UserRoleType::ROLE_TRADINGDESK);

	auto ret = std::make_shared<VectorDO<PortfolioDO>>();

	if (auto pWorkerProc = MessageUtility::AbstractWorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto pPortfoliorMap = pWorkerProc->PricingDataContext()->GetPortfolioMap();

		auto& userid = session->getUserInfo().getUserId();

		auto it = pPortfoliorMap->find(userid);

		if (it != pPortfoliorMap->end())
		{
			for (auto pair : it->second)
			{
				ret->push_back(pair.second);
			}
		}
	}

	return ret;
}
