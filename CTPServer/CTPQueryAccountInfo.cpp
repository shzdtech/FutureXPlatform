/***********************************************************************
 * Module:  CTpData.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:37:12
 * Purpose: Implementation of the class CTpData
 ***********************************************************************/

#include "CTPQueryAccountInfo.h"
#include "CTPRawAPI.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../message/GlobalProcessorRegistry.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include "../message/DefMessageID.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"

#include <thread>

#include "../dataobject/AccountInfoDO.h"

#include "CTPUtility.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryAccountInfo::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPQueryAccountInfo::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryAccountInfo::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (MapDO<std::string>*)reqDO.get();
	auto& brokeid = session->getUserInfo()->getBrokerId();
	auto& userid = session->getUserInfo()->getInvestorId();

	CThostFtdcQryTradingAccountField req{};
	std::strcpy(req.BrokerID, brokeid.data());
	std::strcpy(req.InvestorID, userid.data());

	if (auto wkProcPtr = std::static_pointer_cast<CTPTradeWorkerProcessor>
		(GlobalProcessorRegistry::FindProcessor(CTPWorkerProcessorID::TRADE_SHARED_ACCOUNT)))
	{
		auto& accountInfoVec = wkProcPtr->GetAccountInfo(session->getUserInfo()->getInvestorId());

		if (accountInfoVec.size() < 1)
		{
			int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryTradingAccount(&req, reqDO->SerialId);
			CTPUtility::CheckReturnError(iRet);
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}

		if (accountInfoVec.begin() != accountInfoVec.end())
		{
			auto lastit = std::prev(accountInfoVec.end());
			for (auto it = accountInfoVec.begin(); it != accountInfoVec.end(); it++)
			{
				it->SerialId = stdo->SerialId;
				it->HasMore = it != lastit;
				wkProcPtr->SendDataObject(session, MSG_ID_QUERY_ACCOUNT_INFO, std::make_shared<AccountInfoDO>(*it));
			}
		}
		else
		{
			throw NotFoundException();
		}
	}
	else
	{
		int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryTradingAccount(&req, reqDO->SerialId);
		CTPUtility::CheckReturnError(iRet);
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryAccountInfo::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryAccountInfo::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryAccountInfo::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);

	dataobj_ptr ret;

	if (auto pData = (CThostFtdcTradingAccountField*)rawRespParams[0])
	{
		auto pDO = new AccountInfoDO;
		ret.reset(pDO);
		pDO->SerialId = serialId;
		pDO->HasMore = !*(bool*)rawRespParams[3];

		pDO->BrokerID = pData->BrokerID;
		pDO->AccountID = pData->AccountID;
		pDO->PreMortgage = pData->PreMortgage;
		pDO->PreCredit = pData->PreCredit;
		pDO->PreDeposit = pData->PreDeposit;
		pDO->PreBalance = pData->PreBalance;
		pDO->PreMargin = pData->PreMargin;
		pDO->InterestBase = pData->InterestBase;
		pDO->Interest = pData->Interest;
		pDO->Deposit = pData->Deposit;
		pDO->Withdraw = pData->Withdraw;
		pDO->FrozenMargin = pData->FrozenMargin;
		pDO->FrozenCash = pData->FrozenCash;
		pDO->FrozenCommission = pData->FrozenCommission;
		pDO->CurrMargin = pData->CurrMargin;
		pDO->CashIn = pData->CashIn;
		pDO->Commission = pData->Commission;
		pDO->CloseProfit = pData->CloseProfit;
		pDO->PositionProfit = pData->PositionProfit;
		pDO->Balance = pData->Balance;
		pDO->Available = pData->Available;
		pDO->WithdrawQuota = pData->WithdrawQuota;
		pDO->Reserve = pData->Reserve;
		pDO->TradingDay = pData->TradingDay;
		pDO->SettlementID = pData->SettlementID;
		pDO->Credit = pData->Credit;
		pDO->Mortgage = pData->Mortgage;
		pDO->ExchangeMargin = pData->ExchangeMargin;
		pDO->DeliveryMargin = pData->DeliveryMargin;
		pDO->ExchangeDeliveryMargin = pData->ExchangeDeliveryMargin;
		pDO->ReserveBalance = pData->Reserve;

		if (auto wkProcPtr = std::static_pointer_cast<CTPTradeWorkerProcessor>
			(GlobalProcessorRegistry::FindProcessor(CTPWorkerProcessorID::TRADE_SHARED_ACCOUNT)))
		{
			wkProcPtr->GetAccountInfo(session->getUserInfo()->getUserId()).push_back(*pDO);
		}
	}

	return ret;
}