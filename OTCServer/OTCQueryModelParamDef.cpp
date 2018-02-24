#include "OTCQueryModelParamDef.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"

#include "../message/BizError.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/ResultDO.h"

#include "../databaseop/ModelParamsDAO.h"
#include "../bizutility/ModelParamDefCache.h"



dataobj_ptr OTCQueryModelParamDef::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	CheckRolePermission(session, ROLE_TRADINGDESK);

	ModelParamDefDO_Ptr ret;
	auto& userId = session->getUserInfo().getUserId();

	auto pModelName = (StringMapDO<std::string>*)reqDO.get();
	if (!pModelName->empty())
	{
		auto& modelName = pModelName->begin()->first;
		ret = ModelParamDefCache::FindOrRetrieveModelParamDef(modelName);

		if(!ret)
			throw NotFoundException(modelName);
	}
	else
	{
		throw NotFoundException();
	}


	return ret;
}

dataobj_ptr OTCQueryModelParamDef::HandleResponse(const uint32_t serialId, const param_vector & rawRespParams, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}
