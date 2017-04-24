#include "OTCNewPortfolio.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"

#include "../message/BizError.h"

#include "../message/message_macro.h"
#include "../pricingengine/IPricingDataContext.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/ResultDO.h"
#include "../dataobject/UserInfoDO.h"

#include "../databaseop/PortfolioDAO.h"

dataobj_ptr OTCNewPortfolio::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto vecDO_Ptr = (VectorDO<PortfolioDO>*)reqDO.get();

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto& userid = session->getUserInfo().getUserId();
		auto pUserInfo = (UserInfoDO*)session->getUserInfo().getExtInfo().get();
		for (auto& portfolioDO : *vecDO_Ptr)
		{
			portfolioDO.SetUserID(userid);
			PortfolioDAO::UpsertPortofolio(portfolioDO);
			if (pUserInfo)
			{
				pUserInfo->Portfolios.emplace(portfolioDO.PortfolioID());
			}
		}
	}

	return std::make_shared<ResultDO>();
}
