#include "OTCQueryModelParams.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"

#include "../message/BizError.h"

#include "../message/message_macro.h"
#include "../pricingengine/IPricingDataContext.h"
#include "../pricingengine/ComplexAlgoirthmManager.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/ModelParamsDO.h"
#include "../dataobject/ResultDO.h"

#include "../databaseop/ModelParamsDAO.h"
#include "../bizutility/StrategyModelCache.h"



dataobj_ptr OTCQueryModelParams::HandleRequest(const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	CheckLogin(session);

	dataobj_ptr ret;

	auto pModelParam = (ModelParamsDO*)reqDO.get();
	pModelParam->SetUserID(session->getUserInfo()->getUserId());
	ret = StrategyModelCache::FindModel(*pModelParam);
	if (!ret)
	{
		throw NotFoundException(pModelParam->InstanceName);
	}

	return ret;
}
