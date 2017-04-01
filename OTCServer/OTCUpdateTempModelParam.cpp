#include "OTCUpdateTempModelParam.h"
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


dataobj_ptr OTCUpdateTempModelParam::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr & msgProcessor, const IMessageSession_Ptr & session)
{
	CheckLogin(session);

	auto modelParam_ptr = std::static_pointer_cast<ModelParamsDO>(reqDO);
	modelParam_ptr->SetUserID(session->getUserInfo().getUserId());

	if (modelParam_ptr->Params.empty())
	{
		StrategyModelCache::RemoveTempModel(*modelParam_ptr);
	}
	else
	{
		ModelParamsDO_Ptr modelptr = StrategyModelCache::FindTempModel(*modelParam_ptr);
		if (!modelptr)
		{
			if (modelptr = StrategyModelCache::FindOrRetrieveModel(*modelParam_ptr))
				modelptr = std::make_shared<ModelParamsDO>(*modelptr);
			else
				throw NotFoundException(modelParam_ptr->InstanceName);
		}

		if (auto modelAlg = ComplexAlgoirthmManager::Instance()->FindModel(modelptr->Model))
		{
			auto& paramsMap = modelAlg->DefaultParams();

			for (auto it = modelParam_ptr->Params.begin(); it != modelParam_ptr->Params.end(); it++)
			{
				if (paramsMap.find(it->first) != paramsMap.end())
					modelptr->Params[it->first] = it->second;
			}
			modelAlg->ParseParams(modelptr->Params, modelptr->ParsedParams);

			StrategyModelCache::InsertTempModel(modelptr);

			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
			{
				auto pStrategyMap = pWorkerProc->PricingDataContext()->GetStrategyMap();
				auto it = pStrategyMap->lock_table();
				for (auto& pair : it)
				{
					auto& strategy_ptr = pair.second;
					auto ivmModel_Ptr = strategy_ptr->IVModel;
					auto volModel_Ptr = strategy_ptr->VolModel;
					if ((ivmModel_Ptr && ivmModel_Ptr->operator==(*modelParam_ptr)) ||
						(volModel_Ptr && volModel_Ptr->operator==(*modelParam_ptr)))
					{
						pWorkerProc->TriggerTadingDeskParams(*strategy_ptr);
					}
				}
			}
		}
	}

	return std::make_shared<ResultDO>();
}
