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

std::shared_ptr<UserInfoDO> CTPTradeLoginHandler::LoginFromServer(const IMessageProcessor_Ptr& msgProcessor,
	const std::shared_ptr<UserInfoDO> & userInfoDO_Ptr, uint requestId, const std::string& serverName)
{
	auto& session = msgProcessor->getMessageSession();
	auto& userInfo = session->getUserInfo();

	auto pProcessor = GetTradeProcessor(msgProcessor);

	PositionPortfolioMap::LoadUserPortfolio(userInfo.getUserId());

	if (auto pWorkerProc = GetWorkerProcessor(msgProcessor))
	{
		pWorkerProc->RegisterLoggedSession(session);
	}

	LoginFromDB(msgProcessor);

	LoadUserRiskModel(msgProcessor);

	if (userInfoDO_Ptr->ExchangeUser.empty())
	{
		pProcessor->getMessageSession()->setLoginTimeStamp();
		return userInfoDO_Ptr;
	}

	int ret = 0;
	std::string server = serverName;
	if (server.empty())
		msgProcessor->getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_TD, server);

	ExchangeRouterDO exDO;
	ExchangeRouterTable::TryFind(server, exDO);

	CThostFtdcReqUserLoginField req{};
	std::strncpy(req.BrokerID, userInfo.getBrokerId().data(), sizeof(req.BrokerID));
	std::strncpy(req.UserID, userInfo.getInvestorId().data(), sizeof(req.UserID));
	std::strncpy(req.Password, userInfoDO_Ptr->ExchangePassword.data(), sizeof(req.Password));

	pProcessor->CreateCTPAPI(userInfo.getInvestorId(), exDO.Address);

	CThostFtdcReqAuthenticateField reqAuth{};

	if (!exDO.AuthCode.empty())
	{
		std::strncpy(reqAuth.BrokerID, exDO.BrokeID.data(), sizeof(reqAuth.BrokerID));
		std::strncpy(reqAuth.UserID, userInfo.getInvestorId().data(), sizeof(reqAuth.UserID));
		std::strncpy(reqAuth.AuthCode, exDO.AuthCode.data(), sizeof(reqAuth.AuthCode));
		std::strncpy(reqAuth.UserProductInfo, exDO.ProductInfo.data(), sizeof(reqAuth.UserProductInfo));

		ret = pProcessor->RawAPI_Ptr()->TdAPIProxy()->get()->ReqAuthenticate(&reqAuth, 0);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if (ret == 0)
		ret = pProcessor->RawAPI_Ptr()->TdAPIProxy()->get()->ReqUserLogin(&req, requestId);

	// try after market server
	if (ret == -1)
	{
		msgProcessor->getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_TD_AM, server);
		if (ExchangeRouterTable::TryFind(server, exDO))
		{
			pProcessor->CreateCTPAPI(userInfo.getInvestorId(), exDO.Address);
			if (exDO.AuthCode.empty())
			{
				ret = pProcessor->RawAPI_Ptr()->TdAPIProxy()->get()->ReqAuthenticate(&reqAuth, 0);
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			ret = pProcessor->RawAPI_Ptr()->TdAPIProxy()->get()->ReqUserLogin(&req, requestId);
		}
	}

	CTPUtility::CheckReturnError(ret);

	return nullptr;
}

CTPTradeProcessor* CTPTradeLoginHandler::GetTradeProcessor(const IMessageProcessor_Ptr& msgProcessor)
{
	return (CTPTradeProcessor*)msgProcessor.get();
}

CTPTradeWorkerProcessor* CTPTradeLoginHandler::GetWorkerProcessor(const IMessageProcessor_Ptr& msgProcessor)
{
	return MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor);
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
					if (auto modeldef_ptr = ModelParamDefCache::FindOrRetrieveModelParamDef(model_ptr->Model))
					{
						for (auto pair : modeldef_ptr->ModelDefMap)
						{
							model_ptr->Params.emplace(pair.first, pair.second.DefaultVal);
						}
						alg_ptr->ParseParams(model_ptr->Params, model_ptr->ParsedParams);

						PortfolioKey portfolioID(model_ptr->ParamString[RiskModelParams::__portfolio__], userId);

						if (auto pRiskModelParam = (RiskModelParams*)model_ptr->ParsedParams.get())
						{

							if (pRiskModelParam->PreTrade)
								RiskContext::Instance()->InsertPreTradeUserModel(portfolioID, model_ptr->InstanceName);
							else
								RiskContext::Instance()->InsertPostTradeUserModel(portfolioID, model_ptr->InstanceName);
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
	std::strncpy(reqsettle.BrokerID, session->getUserInfo().getBrokerId().data(), sizeof(reqsettle.BrokerID));
	std::strncpy(reqsettle.InvestorID, session->getUserInfo().getInvestorId().data(), sizeof(reqsettle.InvestorID));
	((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqSettlementInfoConfirm(&reqsettle, 0);

	LoginFromDB(msgProcessor);

	((CTPTradeProcessor*)msgProcessor.get())->QueryUserPositionAsyncIfNeed();

	return ret;
}
