#include "OTCQueryPosition.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../common/Attribute_Key.h"

#include "../message/BizError.h"
#include "../message/MessageUtility.h"
#include "../message/DefMessageID.h"

#include "../message/message_macro.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/UserInfoDO.h"

#include "../pricingengine/IPricingDataContext.h"



dataobj_ptr OTCQueryPosition::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	if (auto pWorkerProc = MessageUtility::AbstractWorkerProcessorPtr<OTCTradeWorkerProcessor>(msgProcessor))
	{
		auto& userid = session->getUserInfo().getUserId();

		auto positionMap = pWorkerProc->GetOTCOrderManager().GetPositionContext().GetPortfolioPositionsByUser(userid);

		if (positionMap.empty())
		{
			throw NotFoundException();
		}

		bool found = false;

		auto locktb = positionMap.map()->lock_table();
		auto endit = locktb.end();
		for (auto it = locktb.begin(); it != endit;)
		{
			if (!it->second.empty())
			{
				auto clock = it->second.map()->lock_table();
				++it;

				auto cendit = clock.end();
				for (auto cit = clock.begin(); cit != cendit;)
				{
					auto positionDO_Ptr = std::make_shared<UserPositionExDO>(*cit->second);
					positionDO_Ptr->HasMore = ++cit != cendit && it != endit;
					OnResponseProcMacro(msgProcessor, MSG_ID_QUERY_POSITION, reqDO->SerialId, &positionDO_Ptr);

					found = true;
				}
			}
			else
			{
				++it;
			}
		}

		if (!found)
			throw NotFoundException();

	}

	return nullptr;
}

dataobj_ptr OTCQueryPosition::HandleResponse(const uint32_t serialId, const param_vector & rawRespParams, IRawAPI * rawAPI, const IMessageProcessor_Ptr & msgProcessor, const IMessageSession_Ptr & session)
{
	return *(UserPositionExDO_Ptr*)rawRespParams[0];
}
