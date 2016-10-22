#include "OTCQueryPortfolio.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../common/Attribute_Key.h"

#include "../message/BizError.h"
#include "../message/MessageUtility.h"

#include "../message/message_macro.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/UserInfoDO.h"

#include "../pricingengine/IPricingDataContext.h"



dataobj_ptr OTCQueryPortfolio::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	CheckLogin(session);

	if (auto userInfoPtr = std::static_pointer_cast<UserInfoDO>(session->getUserInfo()->getExtInfo()))
	{
		ThrowNotFoundExceptionIfEmpty(userInfoPtr->Portfolios);

		return std::make_shared<VectorDO<PortfolioDO>>(*userInfoPtr->Portfolios);
	}

	return nullptr;
}
