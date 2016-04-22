/***********************************************************************
 * Module:  CTpData.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:37:12
 * Purpose: Implementation of the class CTpData
 ***********************************************************************/

#include "CTPQueryAccountInfo.h"
#include "CTPRawAPI.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include <glog/logging.h>

#include "../dataobject/AccountInfoDO.h"

#include "CTPUtility.h"
////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryAccountInfo::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryAccountInfo::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryAccountInfo::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();
	auto& data = stdo->Data;
	auto& brokeid = session->getUserInfo()->getBrokerId();
	auto& userid = session->getUserInfo()->getInvestorId();

	CThostFtdcQryTradingAccountField req;
	std::memset(&req, 0, sizeof(req));
	std::strcpy(req.BrokerID, brokeid.data());
	std::strcpy(req.InvestorID, userid.data());
	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryTradingAccount(&req, ++_requestIdGen);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryAccountInfo::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryAccountInfo::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryAccountInfo::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);

	dataobj_ptr ret;

	if (auto pData = (CThostFtdcTradingAccountField*)rawRespParams[0])
	{
		auto pDO = new AccountInfoDO;
		ret.reset(pDO);

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
	}

	return ret;
}