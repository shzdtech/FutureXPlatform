/***********************************************************************
 * Module:  TradingDeskContextBuilder.cpp
 * Author:  milk
 * Modified: 2016年2月8日 23:37:07
 * Purpose: Implementation of the class TradingDeskContextBuilder
 ***********************************************************************/

#include "TradingDeskContextBuilder.h"

#include "../databaseop/PortfolioDAO.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"

#include "../pricingengine/PricingContext.h"

#include "../CTPServer/Attribute_Key.h"

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
	if (auto portfolioDOVec_Ptr = PortfolioDAO::FindPortfolioByUser(
		pSession->getUserInfo()->getUserId()))
	{
		auto pPortfoliorMap = PricingContext::Instance()->GetPortfolioDOMap();

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

		auto portfolioVec_Ptr = std::make_shared<std::vector<PortfolioKey>>();
		portfolioVec_Ptr->assign(portfolioDOVec_Ptr->begin(), portfolioDOVec_Ptr->end());
		pSession->getContext()->setAttribute(STR_KEY_USER_PORTFOLIO, portfolioVec_Ptr);
	}
}

void TradingDeskContextBuilder::LoadContractParam(ISession* pSession)
{
	auto contractVec_Ptr = 
		ContractDAO::FindContractParamByUser(pSession->getUserInfo()->getUserId());

	auto contractMap = PricingContext::Instance()->GetContractMap();

	for (auto& con : *contractVec_Ptr)
	{
		auto it = contractMap->find(con);
		if (it != contractMap->end())
		{
			con = it->second;
		}
		else
		{
			contractMap->emplace(con, con);
		}

		auto conParamVec_Ptr = std::make_shared<std::vector<ContractKey>>();
		conParamVec_Ptr->assign(contractVec_Ptr->begin(), contractVec_Ptr->end());
		pSession->getContext()->setAttribute(STR_KEY_USER_CONTRACT_PARAM, conParamVec_Ptr);
	}
}

void TradingDeskContextBuilder::LoadStrategy(ISession* pSession)
{
	if (auto sDOVec_Ptr = StrategyContractDAO::FindStrategyContractByUser(
		pSession->getUserInfo()->getUserId()))
	{
		auto strategyMap = PricingContext::Instance()->GetStrategyMap();

		for (auto& strategy : *sDOVec_Ptr)
		{
			auto it = strategyMap->find(strategy);
			if (it != strategyMap->end())
			{
				strategy = it->second;
			}
			else
			{
				strategyMap->emplace(strategy, strategy);
			}
		}

		auto strategyVec_Ptr = std::make_shared<std::vector<ContractKey>>();
		strategyVec_Ptr->assign(sDOVec_Ptr->begin(), sDOVec_Ptr->end());
		pSession->getContext()->setAttribute(STR_KEY_USER_STRATEGY, strategyVec_Ptr);
	}
}