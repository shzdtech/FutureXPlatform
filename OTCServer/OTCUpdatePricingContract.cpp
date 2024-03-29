#include "OTCUpdatePricingContract.h"
#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/DefMessageID.h"
#include "../message/BizError.h"
#include "../message/message_macro.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/ResultDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../databaseop/StrategyContractDAO.h"

#include "../bizutility/ModelParamsCache.h"
#include "../pricingengine/ComplexAlgoirthmManager.h"

dataobj_ptr OTCUpdatePricingContract::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr & msgProcessor, const IMessageSession_Ptr & session)
{
	CheckLogin(session);
	CheckRolePermission(session, UserRoleType::ROLE_TRADINGDESK);

	auto ret = std::make_shared<VectorDO<StrategyContractDO>>();

	if (auto pWorkerProc = MessageUtility::AbstractWorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto pStrategyDO = (StrategyContractDO*)reqDO.get();
		pStrategyDO->SetUserID(session->getUserInfo().getUserId());
		auto pStrategyContractMap = pWorkerProc->PricingDataContext()->GetStrategyMap();
		auto pStrategyMap = pWorkerProc->PricingDataContext()->GetUserStrategyMap()->tryfind(pStrategyDO->UserID());
		if (!pStrategyMap)
			throw NotFoundException("User " + pStrategyDO->UserID() + " does not have strategies!");

		auto pUserStrategyMap_ptr = pStrategyMap->tryfind(pStrategyDO->StrategyName);
		if (!pUserStrategyMap_ptr)
			throw NotFoundException("Stragey symbol " + pStrategyDO->StrategyName + " is not found!");

		if (pStrategyDO->PricingContracts)
		{
			std::vector<StrategyContractDO_Ptr> updateVec;
			for (auto& pair : pUserStrategyMap_ptr->get()->lock_table())
			{
				StrategyContractDO_Ptr scDO_ptr = std::make_shared<StrategyContractDO>(*pair.second);
				scDO_ptr->PricingContracts = std::make_shared<StrategyPricingContract>(*pair.second->PricingContracts);

				pWorkerProc->UnsubscribePricingContracts(*scDO_ptr, *scDO_ptr->PricingContracts);
				scDO_ptr->PricingContracts->PricingContracts.clear();

				for (auto& contract : pStrategyDO->PricingContracts->PricingContracts)
				{
					scDO_ptr->PricingContracts->PricingContracts.push_back(contract);
				}

				pWorkerProc->SubscribePricingContracts(*scDO_ptr, *scDO_ptr->PricingContracts);
				updateVec.push_back(std::move(scDO_ptr));
			}

			for (auto& st_ptr : updateVec)
			{
				pUserStrategyMap_ptr->get()->update(*st_ptr, st_ptr);
				pStrategyContractMap->update(*st_ptr, st_ptr);
				pWorkerProc->TriggerPricingByStrategy(*st_ptr);
				pWorkerProc->GetOTCTradeWorkerProcessor()->TriggerAutoOrderUpdating(*st_ptr);
			}

			for (auto& st_ptr : updateVec)
			{
				StrategyContractDAO::UpdatePricingContract(*st_ptr, st_ptr->StrategyName, PM, *st_ptr->PricingContracts);
			}
		}

		if (pStrategyDO->IVMContracts)
		{
			std::vector<StrategyContractDO_Ptr> updateVec;

			for (auto& pair : pUserStrategyMap_ptr->get()->lock_table())
			{
				StrategyContractDO_Ptr scDO_ptr = std::make_shared<StrategyContractDO>(*pair.second);
				scDO_ptr->IVMContracts = std::make_shared<StrategyPricingContract>(*pair.second->IVMContracts);

				pWorkerProc->UnsubscribePricingContracts(*scDO_ptr, *scDO_ptr->IVMContracts);
				scDO_ptr->IVMContracts->PricingContracts.clear();

				for (auto& contract : pStrategyDO->IVMContracts->PricingContracts)
				{
					scDO_ptr->IVMContracts->PricingContracts.push_back(contract);
				}

				pWorkerProc->SubscribePricingContracts(*scDO_ptr, *scDO_ptr->IVMContracts);
				updateVec.push_back(std::move(scDO_ptr));
			}

			for (auto& st_ptr : updateVec)
			{
				pUserStrategyMap_ptr->get()->update(*st_ptr, st_ptr);
				pStrategyContractMap->update(*st_ptr, st_ptr);
				pWorkerProc->TriggerPricingByStrategy(*st_ptr);
				pWorkerProc->GetOTCTradeWorkerProcessor()->TriggerAutoOrderUpdating(*st_ptr);
			}

			for (auto& st_ptr : updateVec)
			{
				StrategyContractDAO::UpdatePricingContract(*st_ptr, st_ptr->StrategyName, IVM, *st_ptr->IVMContracts);
			}
		}

		if (pStrategyDO->VolContracts)
		{
			std::vector<StrategyContractDO_Ptr> updateVec;

			for (auto& pair : pUserStrategyMap_ptr->get()->lock_table())
			{
				StrategyContractDO_Ptr scDO_ptr = std::make_shared<StrategyContractDO>(*pair.second);
				scDO_ptr->VolContracts = std::make_shared<StrategyPricingContract>(*pair.second->VolContracts);

				pWorkerProc->UnsubscribePricingContracts(*scDO_ptr, *scDO_ptr->VolContracts);
				scDO_ptr->VolContracts->PricingContracts.clear();

				for (auto& contract : pStrategyDO->VolContracts->PricingContracts)
				{
					scDO_ptr->VolContracts->PricingContracts.push_back(contract);
				}

				pWorkerProc->SubscribePricingContracts(*scDO_ptr, *scDO_ptr->VolContracts);
				updateVec.push_back(std::move(scDO_ptr));
			}

			for (auto& st_ptr : updateVec)
			{
				pUserStrategyMap_ptr->get()->update(*st_ptr, st_ptr);
				pStrategyContractMap->update(*st_ptr, st_ptr);
				pWorkerProc->TriggerPricingByStrategy(*st_ptr);
				pWorkerProc->GetOTCTradeWorkerProcessor()->TriggerAutoOrderUpdating(*st_ptr);
			}

			for (auto& st_ptr : updateVec)
			{
				StrategyContractDAO::UpdatePricingContract(*st_ptr, st_ptr->StrategyName, VM, *st_ptr->VolContracts);
			}
		}

		bool updated = false;
		if (pStrategyDO->PricingModel)
		{
			if (auto model_ptr = ModelParamsCache::FindModel(ModelKey(pStrategyDO->PricingModel->InstanceName, pStrategyDO->UserID())))
			{
				if (!model_ptr->ParsedParams)
				{
					if (auto modelAlg = ComplexAlgoirthmManager::Instance()->FindModel(model_ptr->Model))
						modelAlg->ParseParams(model_ptr->Params, model_ptr->ParsedParams);
				}

				updated = true;
				std::vector<StrategyContractDO_Ptr> updateVec;

				for (auto& pair : pUserStrategyMap_ptr->get()->lock_table())
				{
					StrategyContractDO_Ptr scDO_ptr = std::make_shared<StrategyContractDO>(*pair.second);
					if (scDO_ptr->PricingModel)
					{
						scDO_ptr->PricingModel = model_ptr;
						updateVec.push_back(std::move(scDO_ptr));
					}
				}

				for (auto& st_ptr : updateVec)
				{
					pUserStrategyMap_ptr->get()->update(*st_ptr, st_ptr);
					pStrategyContractMap->update(*st_ptr, st_ptr);
					pWorkerProc->TriggerPricingByStrategy(*st_ptr);
					pWorkerProc->GetOTCTradeWorkerProcessor()->TriggerAutoOrderUpdating(*st_ptr);
				}
			}
		}

		if (pStrategyDO->IVModel)
		{
			if (auto model_ptr = ModelParamsCache::FindModel(ModelKey(pStrategyDO->IVModel->InstanceName, pStrategyDO->UserID())))
			{
				if (!model_ptr->ParsedParams)
				{
					if (auto modelAlg = ComplexAlgoirthmManager::Instance()->FindModel(model_ptr->Model))
						modelAlg->ParseParams(model_ptr->Params, model_ptr->ParsedParams);
				}

				updated = true;
				std::vector<StrategyContractDO_Ptr> updateVec;

				for (auto& pair : pUserStrategyMap_ptr->get()->lock_table())
				{
					StrategyContractDO_Ptr scDO_ptr = std::make_shared<StrategyContractDO>(*pair.second);
					if (scDO_ptr->IVModel)
					{
						scDO_ptr->IVModel = model_ptr;
						updateVec.push_back(std::move(scDO_ptr));
					}
				}

				for (auto& st_ptr : updateVec)
				{
					pUserStrategyMap_ptr->get()->update(*st_ptr, st_ptr);
					pStrategyContractMap->update(*st_ptr, st_ptr);
					pWorkerProc->TriggerPricingByStrategy(*st_ptr);
					pWorkerProc->GetOTCTradeWorkerProcessor()->TriggerAutoOrderUpdating(*st_ptr);
				}
			}
		}

		if (pStrategyDO->VolModel)
		{
			if (auto model_ptr = ModelParamsCache::FindModel(ModelKey(pStrategyDO->VolModel->InstanceName, pStrategyDO->UserID())))
			{
				if (!model_ptr->ParsedParams)
				{
					if (auto modelAlg = ComplexAlgoirthmManager::Instance()->FindModel(model_ptr->Model))
						modelAlg->ParseParams(model_ptr->Params, model_ptr->ParsedParams);
				}

				updated = true;
				std::vector<StrategyContractDO_Ptr> updateVec;

				for (auto& pair : pUserStrategyMap_ptr->get()->lock_table())
				{
					StrategyContractDO_Ptr scDO_ptr = std::make_shared<StrategyContractDO>(*pair.second);
					if (scDO_ptr->VolModel)
					{
						scDO_ptr->VolModel = model_ptr;
						updateVec.push_back(std::move(scDO_ptr));
					}
				}

				for (auto& st_ptr : updateVec)
				{
					pUserStrategyMap_ptr->get()->update(*st_ptr, st_ptr);
					pStrategyContractMap->update(*st_ptr, st_ptr);
					pWorkerProc->TriggerPricingByStrategy(*st_ptr);
					pWorkerProc->GetOTCTradeWorkerProcessor()->TriggerAutoOrderUpdating(*st_ptr);
				}
			}
		}

		if (updated)
		{
			StrategyContractDAO::UpdateStrategyModel(*pStrategyDO);
		}
	}

	return ret;
}
