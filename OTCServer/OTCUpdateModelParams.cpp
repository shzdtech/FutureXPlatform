#include "OTCUpdateModelParams.h"
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



dataobj_ptr OTCUpdateModelParams::HandleRequest(const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	CheckLogin(session);

	auto pModelParam = (ModelParamsDO*)reqDO.get();
	pModelParam->SetUserID(session->getUserInfo()->getUserId());

	if (auto modelptr = StrategyModelCache::FindModel(*pModelParam))
	{
		if (auto modelAlg = ComplexAlgoirthmManager::Instance()->FindModel(modelptr->Model))
		{
			auto& paramsMap = modelAlg->DefaultParams();
			pModelParam->Model = modelptr->Model;
			for (auto it = pModelParam->Params.begin(); it != pModelParam->Params.end();)
			{
				if (paramsMap.find(it->first) != paramsMap.end())
				{
					modelptr->Params[it->first] = it->second;
					it++;
				}
				else
				{
					it = pModelParam->Params.erase(it);
				}				
			}
			modelptr->ParsedParams = modelAlg->ParseParams(modelptr->Params);
		}

		ModelParamsDAO::SaveModelParams(*pModelParam);
	}
	else
	{
		throw NotFoundException(pModelParam->InstanceName);
	}

	return std::make_shared<ResultDO>(reqDO->SerialId);
}
