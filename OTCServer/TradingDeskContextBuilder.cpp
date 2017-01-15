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
	auto strategyVec_Ptr = std::make_shared<std::vector<ContractKey>>();
	session->getContext()->setAttribute(STR_KEY_USER_STRATEGY, strategyVec_Ptr);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		for (auto productType : pWorkerProc->GetStrategyProductTypes())
		{
			auto& userid = session->getUserInfo().getUserId();

			auto strategyMap = pWorkerProc->PricingDataContext()->GetStrategyMap();

			for (auto it = strategyMap->begin(); ; it++)
			{
				it = std::find_if(it, strategyMap->end(),
					[&userid, productType](const std::pair<ContractKey, StrategyContractDO>& pair) { return  pair.second.UserID() == userid && pair.second.ProductType == productType; });

				if (it == strategyMap->end())
					break;

				auto& strategy = it->second;
				if (strategy.IVModel && !strategy.IVModel->ParsedParams)
				{
					if (auto model = ModelAlgorithmManager::Instance()->FindModel(strategy.IVModel->Model))
					{
						auto& params = model->DefaultParams();
						strategy.IVModel->Params.insert(params.begin(), params.end());
						model->ParseParams(strategy.IVModel->Params, strategy.IVModel->ParsedParams);
					}
				}

				if (strategy.PricingModel && !strategy.PricingModel->ParsedParams)
				{
					if (auto model = PricingAlgorithmManager::Instance()->FindModel(strategy.PricingModel->Model))
					{
						auto& params = model->DefaultParams();
						strategy.PricingModel->Params.insert(params.begin(), params.end());
						model->ParseParams(strategy.PricingModel->Params, strategy.PricingModel->ParsedParams);
					}
				}

				if (strategy.VolModel && !strategy.VolModel->ParsedParams)
				{
					if (auto model = ModelAlgorithmManager::Instance()->FindModel(strategy.VolModel->Model))
					{
						auto& params = model->DefaultParams();
						strategy.VolModel->Params.insert(params.begin(), params.end());
						model->ParseParams(strategy.VolModel->Params, strategy.VolModel->ParsedParams);
					}
				}

				strategyVec_Ptr->push_back(strategy);
			}
		}
	}
}