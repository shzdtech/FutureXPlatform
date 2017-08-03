/***********************************************************************
 * Module:  CTPQueryUserBankAccount.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:36:22
 * Purpose: Implementation of the class CTPQueryUserBankAccount
 ***********************************************************************/

#include "CTPQueryUserBankAccount.h"
#include "CTPRawAPI.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../message/MessageUtility.h"
#include "../common/BizErrorIDs.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../dataobject/UserAccountRegisterDO.h"

#include "../message/BizError.h"
#include "../message/DefMessageID.h"

#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"


#include "CTPUtility.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryUserBankAccount::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryUserBankAccount::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryUserBankAccount::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto pDO = (BankOpRequestDO*)reqDO.get();
	CThostFtdcReqQueryAccountField req{};

	auto& investorId = session->getUserInfo().getInvestorId();
	auto& brokerId = session->getUserInfo().getBrokerId();

	std::strncpy(req.BankID, pDO->BankID.data(), sizeof(req.BankID));
	if (pDO->BankBranchID.empty())
		pDO->BankBranchID = "0000";
	std::strncpy(req.BankBranchID, pDO->BankBranchID.data(), sizeof(req.BankBranchID));

	/*if (pDO->BrokerID.empty())
		pDO->BrokerID = session->getUserInfo().getBrokerId();*/
	std::strncpy(req.BrokerID, brokerId.data(), sizeof(req.BrokerID));

	if (pDO->BrokerBranchID.empty())
		pDO->BrokerBranchID = "0000";
	std::strncpy(req.BrokerBranchID, pDO->BrokerBranchID.data(), sizeof(req.BrokerBranchID));

	std::strncpy(req.BankAccount, pDO->BankAccount.data(), sizeof(req.BankAccount));
	std::strncpy(req.BankPassWord, pDO->BankPassword.data(), sizeof(req.BankPassWord));

	if (pDO->AccountID.empty())
		pDO->AccountID = investorId;
	std::strncpy(req.AccountID, pDO->AccountID.data(), sizeof(req.AccountID));

	if (pDO->Password.empty())
		pDO->Password = session->getUserInfo().getPassword();
	std::strncpy(req.Password, pDO->Password.data(), sizeof(req.Password));

	if (pDO->CurrencyID.empty())
		pDO->CurrencyID = "CNY";
	std::strncpy(req.CurrencyID, pDO->CurrencyID.data(), sizeof(req.CurrencyID));

	std::strncpy(req.UserID, session->getUserInfo().getUserId().data(), sizeof(req.UserID));

	req.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck;

	req.SessionID = session->getUserInfo().getSessionId();
	req.RequestID = serialId;

	int iRet = ((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqQueryBankAccountMoneyByFuture(&req, serialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryUserBankAccount::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryUserBankAccount::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryUserBankAccount::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	UserAccountRegisterDO_Ptr ret;
	if (rawRespParams.size() > 1)
	{
		CTPUtility::CheckError(rawRespParams[1]);
		ret = CTPUtility::ParseUserBankAccout((CThostFtdcReqQueryAccountField*)rawRespParams[0]);
	}
	else
	{
		ret = CTPUtility::ParseUserBankAccout((CThostFtdcNotifyQueryAccountField*)rawRespParams[0]);
	}

	return ret;
}