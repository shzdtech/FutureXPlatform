/***********************************************************************
 * Module:  CTPQueryTransferSerial.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:36:22
 * Purpose: Implementation of the class CTPQueryTransferSerial
 ***********************************************************************/

#include "CTPQueryTransferSerial.h"
#include "CTPRawAPI.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../message/MessageUtility.h"
#include "../common/BizErrorIDs.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../dataobject/BankOpResultDO.h"

#include "../message/BizError.h"
#include "../message/DefMessageID.h"

#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"


#include "CTPUtility.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryTransferSerial::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryTransferSerial::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTransferSerial::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	
	auto pDO = (BankOpRequestDO*)reqDO.get();

	CThostFtdcQryTransferSerialField req{};

	auto& investorId = session->getUserInfo().getInvestorId();
	auto& brokerId = session->getUserInfo().getBrokerId();

	if (pDO->AccountID.empty())
		pDO->AccountID = investorId;
	std::strncpy(req.AccountID, pDO->AccountID.data(), sizeof(req.AccountID));

	/*if (pDO->BrokerID.empty())
		pDO->BrokerID = session->getUserInfo().getBrokerId();*/
	std::strncpy(req.BrokerID, brokerId.data(), sizeof(req.BrokerID));

	std::strncpy(req.BankID, pDO->BankID.data(), sizeof(req.BankID));

	if (pDO->CurrencyID.empty())
		pDO->CurrencyID = "CNY";
	std::strncpy(req.CurrencyID, pDO->CurrencyID.data(), sizeof(req.CurrencyID));

	int iRet = ((CTPRawAPI*)rawAPI)->TdAPI->ReqQryTransferSerial(&req, serialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryTransferSerial::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryTransferSerial::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTransferSerial::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);

	auto ret = CTPUtility::ParseRawTransfer((CThostFtdcTransferSerialField*)rawRespParams[0]);
	ret->HasMore = !(*((bool*)rawRespParams[3]));

	return ret;
}