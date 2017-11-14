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
	CheckRolePermission(session, UserRoleType::ROLE_TRADINGDESK);

	auto pPortfolioDO = (PortfolioDO*)reqDO.get();

	if (auto pWorkerProc = MessageUtility::AbstractWorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto& userid = session->getUserInfo().getUserId();
		pPortfolioDO->SetUserID(userid);

		auto pPortfolioMap = pWorkerProc->PricingDataContext()->GetPortfolioMap();
		pPortfolioMap->getorfill(userid).emplace(pPortfolioDO->PortfolioID(), *pPortfolioDO);
		PortfolioDAO::UpsertPortofolio(*pPortfolioDO);
	}

	return reqDO;
}
