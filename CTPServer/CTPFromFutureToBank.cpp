#include "CTPFromFutureToBank.h"
#include "CTPRawAPI.h"
#include "CTPUtility.h"
#include "CTPMapping.h"
#include "CTPConstant.h"
#include "CTPTradeWorkerProcessor.h"

#include "../message/message_macro.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"

dataobj_ptr CTPFromFutureToBank::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckTradeInit((CTPRawAPI*)rawAPI);

	auto pDO = (BankOpRequestDO*)reqDO.get();

	auto& investorId = session->getUserInfo().getInvestorId();
	auto& brokerId = session->getUserInfo().getBrokerId();

	CThostFtdcReqTransferField req{};
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

	req.TradeAmount = pDO->TradeAmount;

	req.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck;

	req.SessionID = session->getUserInfo().getSessionId();
	req.RequestID = serialId;

	int iRet = ((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqFromFutureToBankByFuture(&req, serialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

dataobj_ptr CTPFromFutureToBank::HandleResponse(const uint32_t serialId, const param_vector & rawRespParams, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);

	BankOpResultDO_Ptr ret;
	if (rawRespParams.size() > 1)
	{
		CTPUtility::CheckError(rawRespParams[1]);
		ret = CTPUtility::ParseRawTransfer((CThostFtdcReqTransferField*)rawRespParams[0]);
	}
	else
	{
		ret = CTPUtility::ParseRawTransfer((CThostFtdcRspTransferField*)rawRespParams[0]);
	}

	return ret;
}
