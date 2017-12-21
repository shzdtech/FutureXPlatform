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



dataobj_ptr OTCUpdateModelParams::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	CheckRolePermission(session, UserRoleType::ROLE_TRADINGDESK);

	auto pModelParam = (ModelParamsDO*)reqDO.get();
	pModelParam->SetUserID(session->getUserInfo().getUserId());

	if (pModelParam->Params.empty())
	{
		if (!pModelParam->Model.empty() && !pModelParam->InstanceName.empty())
		{
			ModelParamsDAO::NewUserModel(*pModelParam);
			StrategyModelCache::FindOrRetrieveModel(*pModelParam);
		}
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

			if (auto pWorkerProc = MessageUtility::AbstractWorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
			{
				if (auto pStrategyMap = pWorkerProc->PricingDataContext()->GetUserStrategyMap()->tryfind(pModelParam->UserID()))
				{
					for (auto& pairMap : *pStrategyMap)
					{
						if (auto stMap_ptr = pairMap.second)
						{
							auto it = stMap_ptr->lock_table();
							for (auto& pair : it)
							{
								auto& strategy_ptr = pair.second;
								auto ivmModel_Ptr = strategy_ptr->IVModel;
								auto volModel_Ptr = strategy_ptr->VolModel;
								auto pmModel_Ptr = strategy_ptr->PricingModel;
								if (
									(volModel_Ptr && volModel_Ptr->operator==(*pModelParam)) ||
									(pmModel_Ptr && pmModel_Ptr->operator==(*pModelParam)) ||
									(ivmModel_Ptr && ivmModel_Ptr->operator==(*pModelParam))
									)
								{
									pWorkerProc->TriggerPricingByStrategy(*strategy_ptr);
									pWorkerProc->GetOTCTradeWorkerProcessor()->TriggerAutoOrderUpdating(*strategy_ptr);
								}
							}
						}
					}
				}
			}
		}
		else
		{
			throw NotFoundException(pModelParam->InstanceName);
		}
	}

	return std::make_shared<ResultDO>();
}
