/***********************************************************************
 * Module:  CTPTradeLoginHandler.cpp
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Implementation of the class CTPTradeLoginHandler
 ***********************************************************************/

#include "CTPTradeLoginHandler.h"
#include "CTPTradeProcessor.h"
#include "CTPRawAPI.h"
#include "CTPConstant.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPUtility.h"

#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../utility/TUtil.h"
#include "../common/Attribute_Key.h"
#include "../dataobject/TypedefDO.h"
#include "../litelogger/LiteLogger.h"
#include "../bizutility/PositionPortfolioMap.h"
#include "../bizutility/ModelParamDefCache.h"
#include "../bizutility/ModelParamsCache.h"
#include "../riskmanager/RiskModelAlgorithmManager.h"
#include "../riskmanager/RiskContext.h"
#include "../riskmanager/RiskModelParams.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPTradeLoginHandler::LoginFromServer()
 // Purpose:    Implementation of CTPTradeLoginHandler::LoginFromServer()
 // Return:     int
 ////////////////////////////////////////////////////////////////////////

std::shared_ptr<UserInfoDO> CTPTradeLoginHandler::LoginFromServer(const CTPProcessor_Ptr& msgProcessor,
	const std::shared_ptr<UserInfoDO> & userInfoDO_Ptr, uint requestId, const std::string& serverName)
{
	auto& session = msgProcessor->getMessageSession();
	auto& userInfo = session->getUserInfo();

	PositionPortfolioMap::LoadUserPortfolio(userInfo.getUserId());

	if (auto pWorkerProc = GetWorkerProcessor(msgProcessor))
	{
		pWorkerProc->RegisterLoggedSession(session);
	}

	LoginFromDB(msgProcessor);

	LoadUserRiskModel(msgProcessor);

	if (userInfoDO_Ptr->ExchangeUser.empty())
	{
		msgProcessor->getMessageSession()->setLoginTimeStamp();
		return userInfoDO_Ptr;
	}

	if (auto pWorkerProc = GetWorkerProcessor(msgProcessor))
	{
		pWorkerProc->LoginUserSession(msgProcessor, userInfoDO_Ptr->BrokerId, userInfoDO_Ptr->ExchangeUser, userInfoDO_Ptr->ExchangePassword, serverName);
		msgProcessor->getMessageSession()->setLoginTimeStamp();
		//LoginFromDB(msgProcessor);
	}

	return userInfoDO_Ptr;
}

CTPTradeWorkerProcessorBase* CTPTradeLoginHandler::GetWorkerProcessor(const IMessageProcessor_Ptr& msgProcessor)
{
	return MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessorBase>(msgProcessor);
}

bool CTPTradeLoginHandler::LoginFromDB(const IMessageProcessor_Ptr& msgProcessor)
{
	if (auto pWorkerProc = GetWorkerProcessor(msgProcessor))
	{
		auto session = msgProcessor->getMessageSession();

		if (pWorkerProc->IsLoadPositionFromDB())
		{
			pWorkerProc->LoadPositonFromDatabase(session->getUserInfo().getUserId(),
				std::to_string(session->getUserInfo().getTradingDay()));
		}
	}

	return true;
}

void CTPTradeLoginHandler::LoadUserRiskModel(const IMessageProcessor_Ptr & msgProcessor)
{
	if (auto pWorkerProc = GetWorkerProcessor(msgProcessor))
	{
		auto session = msgProcessor->getMessageSession();
		auto& userId = session->getUserInfo().getUserId();

		auto userModels = ModelParamsCache::FindModelsByUser(userId);
		if (!userModels)
		{
			ModelParamsCache::Load(userId);
			userModels = ModelParamsCache::FindModelsByUser(userId);
		}

		if (userModels)
		{
			for (auto model_ptr : *userModels)
			{
				if (auto alg_ptr = RiskModelAlgorithmManager::Instance()->FindModel(model_ptr->Model))
				{
					alg_ptr->ParseParams(model_ptr->Params, model_ptr->ParsedParams);

					if (auto pRiskModelParam = (RiskModelParams*)model_ptr->ParsedParams.get())
					{
						auto it = model_ptr->ParamString.find(RiskModelParams::__portfolio__);
						if (it != model_ptr->ParamString.end())
						{
							PortfolioKey portfolioID(it->second, userId);
							if (pRiskModelParam->PreTrade)
								RiskContext::Instance()->InsertPreTradeUserModel(portfolioID, model_ptr->InstanceName);
							else
								RiskContext::Instance()->InsertPostTradeUserModel(portfolioID, model_ptr->InstanceName);
						}
						else
						{
							if (pRiskModelParam->PreTrade)
								RiskContext::Instance()->InsertPreTradeAccountModel(userId, model_ptr->InstanceName);
							else
								RiskContext::Instance()->InsertPostTradeAccountModel(userId, model_ptr->InstanceName);
						}
					}
				}
			}
		}
	}
}

dataobj_ptr CTPTradeLoginHandler::HandleResponse(const uint32_t serialId, const param_vector & rawRespParams, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	dataobj_ptr ret = CTPLoginHandler::HandleResponse(serialId, rawRespParams, rawAPI, msgProcessor, session);

	CThostFtdcSettlementInfoConfirmField reqsettle{};
	((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqSettlementInfoConfirm(&reqsettle, 0);

	((CTPTradeProcessor*)msgProcessor.get())->QueryUserPositionAsyncIfNeed();

	return ret;
}
