/***********************************************************************
 * Module:  TradingDeskContextBuilder.cpp
 * Author:  milk
 * Modified: 2016年2月8日 23:37:07
 * Purpose: Implementation of the class TradingDeskContextBuilder
 ***********************************************************************/

#include "TradingDeskContextBuilder.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../databaseop/PortfolioDAO.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"
#include "../dataobject/UserInfoDO.h"

#include "../common/Attribute_Key.h"
#include "../message/message_macro.h"
#include "../message/MessageUtility.h"
#include "../pricingengine/IPricingDataContext.h"
#include "../pricingengine/PricingAlgorithmManager.h"
#include "../pricingengine/ModelAlgorithmManager.h"

#include "../bizutility/StrategyModelCache.h"

#include <algorithm>

 ////////////////////////////////////////////////////////////////////////
 // Name:       TradingDeskContextBuilder::TradingDeskContextBuilder()
 // Purpose:    Implementation of TradingDeskContextBuilder::TradingDeskContextBuilder()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

TradingDeskContextBuilder::TradingDeskContextBuilder()
{
}

////////////////////////////////////////////////////////////////////////
// Name:       TradingDeskContextBuilder::~TradingDeskContextBuilder()
// Purpose:    Implementation of TradingDeskContextBuilder::~TradingDeskContextBuilder()
// Return:     
////////////////////////////////////////////////////////////////////////

TradingDeskContextBuilder::~TradingDeskContextBuilder()
{
	// TODO : implement
}

////////////////////////////////////////////////////////////////////////
// Name:       TradingDeskContextBuilder::BuildContext(IUserInfo* pUserInfo)
// Purpose:    Implementation of TradingDeskContextBuilder::BuildContext()
// Parameters:
// - pUserInfo
// Return:     void
////////////////////////////////////////////////////////////////////////

void TradingDeskContextBuilder::BuildContext(const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	LoadPortfolio(msgProcessor, session);
	LoadStrategy(msgProcessor, session);
	LoadContractParam(msgProcessor, session);
}


void TradingDeskContextBuilder::LoadPortfolio(const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	if (auto portfolioDOVec_Ptr = PortfolioDAO::FindPortfolioByUser(session->getUserInfo().getUserId()))
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
		{
			auto pPortfoliorMap = pWorkerProc->PricingDataContext()->GetPortfolioMap();

			for (auto& portfolio : *portfolioDOVec_Ptr)
			{
				auto it = pPortfoliorMap->find(portfolio);
				if (it != pPortfoliorMap->end())
				{
					portfolio = it->second;
				}
				else
				{
					pPortfoliorMap->emplace(portfolio, portfolio);
				}
			}

			if (auto userInfoPtr = std::static_pointer_cast<UserInfoDO>(session->getUserInfo().getExtInfo()))
				userInfoPtr->Portfolios = portfolioDOVec_Ptr;
		}
	}
}

void TradingDeskContextBuilder::LoadContractParam(const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto conParamVec_Ptr = std::make_shared<std::vector<ContractKey>>();
	session->getContext()->setAttribute(STR_KEY_USER_CONTRACT_PARAM, conParamVec_Ptr);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto productType = pWorkerProc->GetContractProductType();
		if (auto contractVec_Ptr = StrategyContractDAO::RetrieveContractParamByUser(session->getUserInfo().getUserId(), productType))
		{
			auto contractMap = pWorkerProc->PricingDataContext()->GetContractParamMap();

			for (auto& con : *contractVec_Ptr)
			{
				if (!contractMap->tryfind(con))
				{
					contractMap->emplace(con, con);
				}

				conParamVec_Ptr->assign(contractVec_Ptr->begin(), contractVec_Ptr->end());
			}
		}
	}
}

void TradingDeskContextBuilder::LoadStrategy(const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto strategySet_Ptr = std::make_shared<std::set<ContractKey>>();
	session->getContext()->setAttribute(STR_KEY_USER_STRATEGY, strategySet_Ptr);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto& userid = session->getUserInfo().getUserId();

		auto& strategyMap = pWorkerProc->PricingDataContext()->GetUserStrategyMap()->getorfill(session->getUserInfo().getUserId());

		for (auto& pairMap : strategyMap)
		{
			for (auto& pair : pairMap.second->lock_table())
			{
				auto& strategy_ptr = pair.second;
				if (strategy_ptr->IVModel && !strategy_ptr->IVModel->ParsedParams)
				{
					if (auto model = ModelAlgorithmManager::Instance()->FindModel(strategy_ptr->IVModel->Model))
					{
						auto& params = model->DefaultParams();
						strategy_ptr->IVModel->Params.insert(params.begin(), params.end());
						model->ParseParams(strategy_ptr->IVModel->Params, strategy_ptr->IVModel->ParsedParams);
					}
				}

				if (strategy_ptr->PricingModel && !strategy_ptr->PricingModel->ParsedParams)
				{
					if (auto model = PricingAlgorithmManager::Instance()->FindModel(strategy_ptr->PricingModel->Model))
					{
						auto& params = model->DefaultParams();
						strategy_ptr->PricingModel->Params.insert(params.begin(), params.end());
						model->ParseParams(strategy_ptr->PricingModel->Params, strategy_ptr->PricingModel->ParsedParams);
					}
				}

				if (strategy_ptr->VolModel && !strategy_ptr->VolModel->ParsedParams)
				{
					if (auto model = ModelAlgorithmManager::Instance()->FindModel(strategy_ptr->VolModel->Model))
					{
						auto& params = model->DefaultParams();
						strategy_ptr->VolModel->Params.insert(params.begin(), params.end());
						model->ParseParams(strategy_ptr->VolModel->Params, strategy_ptr->VolModel->ParsedParams);
					}
				}

				strategySet_Ptr->emplace(*strategy_ptr);

				pWorkerProc->SubscribeStrategy(*strategy_ptr);
			}
		}
	}
}