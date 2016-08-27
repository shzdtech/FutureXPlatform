#include "OTCNewPortfolio.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"

#include "../message/BizError.h"

#include "../message/message_macro.h"
#include "../pricingengine/IPricingDataContext.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/ResultDO.h"

#include "../databaseop/PortfolioDAO.h"

dataobj_ptr OTCNewPortfolio::HandleRequest(const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	CheckLogin(session);

	auto vecDO_Ptr = (VectorDO<PortfolioDO>*)reqDO.get();

	if (auto wkProcPtr = MessageUtility::ServerWorkerProcessor<OTCWorkerProcessor>(session->getProcessor()))
	{
		auto& userid = session->getUserInfo()->getUserId();
		for (auto& portfolioDO : *vecDO_Ptr)
		{
			portfolioDO.SetUserID(userid);
			PortfolioDAO::CreatePortofolio(portfolioDO);
		}
	}

	return std::make_shared<ResultDO>(reqDO->SerialId);
}
