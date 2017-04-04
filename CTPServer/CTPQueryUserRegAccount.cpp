/***********************************************************************
 * Module:  CTPQueryUserRegAccount.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:36:22
 * Purpose: Implementation of the class CTPQueryUserRegAccount
 ***********************************************************************/

#include "CTPQueryUserRegAccount.h"
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
 // Name:       CTPQueryUserRegAccount::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryUserRegAccount::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryUserRegAccount::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto pDO = (BankOpRequestDO*)reqDO.get();
	CThostFtdcQryAccountregisterField req{};

	std::strncpy(req.AccountID, pDO->AccountID.data(), sizeof(req.AccountID));
	std::strncpy(req.BankBranchID, pDO->BankBranchID.data(), sizeof(req.BankBranchID));
	std::strncpy(req.BankID, pDO->BankID.data(), sizeof(req.BankID));
	std::strncpy(req.BrokerID, pDO->BrokerID.data(), sizeof(req.BrokerID));
	std::strncpy(req.CurrencyID, pDO->CurrencyID.data(), sizeof(req.CurrencyID));

	int iRet = ((CTPRawAPI*)rawAPI)->TdAPI->ReqQryAccountregister(&req, serialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryUserRegAccount::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryUserRegAccount::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryUserRegAccount::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);

	auto ret = CTPUtility::ParseUserBankAccout((CThostFtdcAccountregisterField*)rawRespParams[0]);
	ret->HasMore = !(*((bool*)rawRespParams[3]));
	return ret;
}