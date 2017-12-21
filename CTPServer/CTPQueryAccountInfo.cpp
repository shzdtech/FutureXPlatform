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

#include "../message/MessageUtility.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include "../message/DefMessageID.h"
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"

#include <thread>

#include "../dataobject/AccountInfoDO.h"

#include "CTPUtility.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryAccountInfo::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryAccountInfo::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryAccountInfo::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto stdo = (StringMapDO<std::string>*)reqDO.get();
	auto& brokeid = session->getUserInfo().getBrokerId();
	auto& investorid = session->getUserInfo().getInvestorId();

	AccountInfoDO_Ptr ret;
	if (CTPUtility::HasTradeInit((CTPRawAPI*)rawAPI))
	{
		CThostFtdcQryTradingAccountField req{};
		std::strncpy(req.BrokerID, brokeid.data(), sizeof(req.BrokerID));
		std::strncpy(req.InvestorID, investorid.data(), sizeof(req.InvestorID));

		int iRet = ((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqQryTradingAccount(&req, serialId);
	}
	else
	{
		ret = std::make_shared<AccountInfoDO>();
	}

	/*AccountInfoDO_Ptr ret;

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		ret = pWorkerProc->GetAccountInfo(session->getUserInfo().getUserId());

		if (!ret)
		{
			ret = pWorkerProc->GetAccountInfo(pWorkerProc->getMessageSession()->getUserInfo().getUserId());
		}

		auto endit = accountInfoMap.end();
		for (auto it = accountInfoMap.begin(); it != endit; it++)
		{
			auto accountptr = std::make_shared<AccountInfoDO>(it->second);
			accountptr->HasMore = std::next(it) != endit;
			
		}
	}*/

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryAccountInfo::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryAccountInfo::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryAccountInfo::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);

	std::shared_ptr<AccountInfoDO> ret;

	if (auto pData = (CThostFtdcTradingAccountField*)rawRespParams[0])
	{
		auto pDO = new AccountInfoDO;
		ret.reset(pDO);
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
		pDO->TradingDay = std::atoi(pData->TradingDay);
		pDO->SettlementID = pData->SettlementID;
		pDO->Credit = pData->Credit;
		pDO->Mortgage = pData->Mortgage;
		pDO->ExchangeMargin = pData->ExchangeMargin;
		pDO->DeliveryMargin = pData->DeliveryMargin;
		pDO->ExchangeDeliveryMargin = pData->ExchangeDeliveryMargin;
		pDO->ReserveBalance = pData->Reserve;

		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			pWorkerProc->UpdateAccountInfo(session->getUserInfo().getUserId(), ret);
		}
	}

	return ret;
}