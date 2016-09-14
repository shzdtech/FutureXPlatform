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

void TradingDeskContextBuilder::BuildContext(ISession* pSession)
{
	LoadPortfolio(pSession);
	LoadStrategy(pSession);
	LoadContractParam(pSession);
}


void TradingDeskContextBuilder::LoadPortfolio(ISession* pSession)
{
	if (auto portfolioDOVec_Ptr = PortfolioDAO::FindPortfolioByUser(pSession->getUserInfo()->getUserId()))
	{
		if (auto wkProcPtr = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(pSession->getProcessor()))
		{
			auto pPortfoliorMap = wkProcPtr->PricingDataContext()->GetPortfolioMap();

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

			if (auto userInfoPtr = std::static_pointer_cast<UserInfoDO>(pSession->getUserInfo()->getExtInfo()))
				userInfoPtr->Portfolios = portfolioDOVec_Ptr;
		}
	}
}

void TradingDeskContextBuilder::LoadContractParam(ISession* pSession)
{
	auto conParamVec_Ptr = std::make_shared<std::vector<ContractKey>>();
	pSession->getContext()->setAttribute(STR_KEY_USER_CONTRACT_PARAM, conParamVec_Ptr);

	if (auto wkProcPtr = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(pSession->getProcessor()))
	{
		if (auto contractVec_Ptr = StrategyContractDAO::RetrieveContractParamByUser(pSession->getUserInfo()->getUserId()))
		{
			auto contractMap = wkProcPtr->PricingDataContext()->GetContractParamMap();

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

void TradingDeskContextBuilder::LoadStrategy(ISession* pSession)
{
	auto strategyVec_Ptr = std::make_shared<std::vector<ContractKey>>();
	pSession->getContext()->setAttribute(STR_KEY_USER_STRATEGY, strategyVec_Ptr);

	if (auto wkProcPtr = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(pSession->getProcessor()))
	{
		for (auto productType : wkProcPtr->GetStrategyProductTypes())
		{
			if (auto sDOVec_Ptr = StrategyContractDAO::FindStrategyContractByUser(
				pSession->getUserInfo()->getUserId(), productType))
			{
				auto strategyMap = wkProcPtr->PricingDataContext()->GetStrategyMap();

				for (auto& strategy : *sDOVec_Ptr)
				{
					auto pStrategyDO = strategyMap->tryfind(strategy);
					if (!pStrategyDO)
					{
						if (auto model = PricingAlgorithmManager::Instance()->FindModel(strategy.PricingModel->Model))
						{
							auto& params = model->DefaultParams();
							strategy.PricingModel->Params.insert(params.begin(), params.end());
						}

						strategyMap->emplace(strategy, strategy);
					}
				}

				strategyVec_Ptr->insert(strategyVec_Ptr->end(), sDOVec_Ptr->begin(), sDOVec_Ptr->end());
			}
		}
	}
}