/***********************************************************************
 * Module:  CTpData.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:37:12
 * Purpose: Implementation of the class CTpData
 ***********************************************************************/

#include "XTQueryAccountInfo.h"
#include "XTRawAPI.h"
#include "XTTradeWorkerProcessor.h"

#include "../message/MessageUtility.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include "../message/DefMessageID.h"
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"

#include <thread>

#include "../dataobject/AccountInfoDO.h"

#include "XTUtility.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       XTQueryAccountInfo::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of XTQueryAccountInfo::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr XTQueryAccountInfo::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	AccountInfoDO_Ptr ret;
	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		ret = pWorkerProc->GetAccountInfo(session->getUserInfo().getUserId());

		//if (!ret)
		//{
		//	ret = pWorkerProc->GetAccountInfo(pWorkerProc->getMessageSession()->getUserInfo().getUserId());
		//}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTQueryAccountInfo::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of XTQueryAccountInfo::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr XTQueryAccountInfo::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	XTUtility::CheckNotFound(rawRespParams[0]);
	XTUtility::CheckError(rawRespParams[1]);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		pWorkerProc->OnRspQryTradingAccount((CThostFtdcTradingAccountField*)rawRespParams[0], (CThostFtdcRspInfoField*)rawRespParams[1],
			*((int*)rawRespParams[2]), *((bool*)rawRespParams[3]));
	}

	return nullptr;
}