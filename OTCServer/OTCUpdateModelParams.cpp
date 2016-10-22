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



dataobj_ptr OTCUpdateModelParams::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	CheckLogin(session);

	auto pModelParam = (ModelParamsDO*)reqDO.get();
	pModelParam->SetUserID(session->getUserInfo()->getUserId());

	if (pModelParam->Params.empty())
	{
		ModelParamsDAO::NewUserModel(*pModelParam);
		StrategyModelCache::FindOrRetrieveModel(*pModelParam);
	}
	else
	{
		if (auto modelptr = StrategyModelCache::FindOrRetrieveModel(*pModelParam))
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
				modelAlg->ParseParams(modelptr->Params, modelptr->ParsedParams);
			}
			ModelParamsDAO::SaveModelParams(*pModelParam);
		}
		else
		{
			throw NotFoundException(pModelParam->InstanceName);
		}
	}

	return std::make_shared<ResultDO>(serialId);
}
