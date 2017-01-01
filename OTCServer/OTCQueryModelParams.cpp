#include "OTCQueryModelParams.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"

#include "../message/BizError.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../pricingengine/IPricingDataContext.h"
#include "../pricingengine/ComplexAlgoirthmManager.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/ModelParamsDO.h"
#include "../dataobject/ResultDO.h"

#include "../databaseop/ModelParamsDAO.h"
#include "../bizutility/StrategyModelCache.h"



dataobj_ptr OTCQueryModelParams::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	dataobj_ptr ret;
	auto& userId = session->getUserInfo().getUserId();

	auto pModelParam = (ModelParamsDO*)reqDO.get();
	if (pModelParam->InstanceName.empty())
	{
		auto models = StrategyModelCache::FindModelsByUser(userId);
		if (!models)
		{
			throw NotFoundException();
		}

		for (auto& model : *models)
		{
			OnResponseProcMacro(msgProcessor, MSG_ID_QUERY_MODELPARAMS, serialId, &model);
		}
	}
	else
	{
		pModelParam->SetUserID(userId);
		ret = StrategyModelCache::FindOrRetrieveModel(*pModelParam);
		if (!ret)
		{
			throw NotFoundException(pModelParam->InstanceName);
		}
	}

	return ret;
}

dataobj_ptr OTCQueryModelParams::HandleResponse(const uint32_t serialId, const param_vector & rawRespParams, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return *(ModelParamsDO_Ptr*)rawRespParams[0];
}
