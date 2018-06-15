/***********************************************************************
 * Module:  CTPQueryTransferBank.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:36:22
 * Purpose: Implementation of the class CTPQueryTransferBank
 ***********************************************************************/

#include "CTPQueryTransferBank.h"
#include "CTPRawAPI.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../message/MessageUtility.h"
#include "../common/BizErrorIDs.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../dataobject/BankDO.h"

#include "../message/BizError.h"
#include "../message/DefMessageID.h"

#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"


#include "CTPUtility.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryTransferBank::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryTransferBank::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTransferBank::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	CTPUtility::CheckTradeInit((CTPRawAPI*)rawAPI);

	auto stdo = (StringMapDO<std::string>*)reqDO.get();

	CThostFtdcQryTransferBankField req{};

	auto bankid = stdo->TryFind(STR_BANK_ID, EMPTY_STRING);
	auto branchid = stdo->TryFind(STR_BRANCH_ID, EMPTY_STRING);

	std::strncpy(req.BankID, bankid.data(), sizeof(req.BankID));
	std::strncpy(req.BankBrchID, branchid.data(), sizeof(req.BankBrchID));

	int iRet = ((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqQryTransferBank(&req, serialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryTransferBank::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryTransferBank::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTransferBank::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);

	auto pDO = new BankDO;
	dataobj_ptr ret(pDO);

	pDO->HasMore = !(*((bool*)rawRespParams[3]));

	if (auto pData = (CThostFtdcTransferBankField*)rawRespParams[0])
	{
		pDO->BankID = pData->BankID;
		pDO->BranchID = pData->BankBrchID;
		pDO->BankName = boost::locale::conv::to_utf<char>(pData->BankName, CHARSET_GB2312);
	}

	return ret;
}