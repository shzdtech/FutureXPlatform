#include "OTCQueryPortfolio.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../common/Attribute_Key.h"

#include "../message/BizError.h"
#include "../message/MessageUtility.h"

#include "../message/message_macro.h"

#include "../dataobject/TemplateDO.h"

#include "../pricingengine/IPricingDataContext.h"



dataobj_ptr OTCQueryPortfolio::HandleRequest(const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	CheckLogin(session);

	auto cpVec_Ptr = std::static_pointer_cast<std::vector<PortfolioDO>>(
		session->getContext()->getAttribute(STR_KEY_USER_PORTFOLIO));

	ThrowNotFoundExceptionIfEmpty(cpVec_Ptr);

	auto portfolioVec_Ptr = std::make_shared<VectorDO<PortfolioDO>>(*cpVec_Ptr);

	return portfolioVec_Ptr;
}
