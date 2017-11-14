/***********************************************************************
 * Module:  CTPOTCTradeLoginHandler.cpp
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Implementation of the class CTPOTCTradeLoginHandler
 ***********************************************************************/

#include "CTPOTCTradeLoginHandler.h"
#include "CTPOTCTradeWorkerProcessor.h"
#include "../CTPServer/CTPRawAPI.h"
#include "../CTPServer/CTPConstant.h"
#include "../CTPServer/CTPUtility.h"

#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../utility/TUtil.h"
#include "../common/Attribute_Key.h"
#include "../dataobject/TypedefDO.h"
#include "../litelogger/LiteLogger.h"


dataobj_ptr CTPOTCTradeLoginHandler::HandleResponse(const uint32_t serialId, const param_vector & rawRespParams, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	dataobj_ptr ret = CTPLoginHandler::HandleResponse(serialId, rawRespParams, rawAPI, msgProcessor, session);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(msgProcessor))
	{
		pWorkerProc->RegisterLoggedSession(session);
	}

	if (auto tdAPI = ((CTPRawAPI*)rawAPI)->TdAPIProxy())
	{
		CThostFtdcSettlementInfoConfirmField reqsettle{};
		std::strncpy(reqsettle.BrokerID, session->getUserInfo().getBrokerId().data(), sizeof(reqsettle.BrokerID));
		std::strncpy(reqsettle.InvestorID, session->getUserInfo().getInvestorId().data(), sizeof(reqsettle.InvestorID));
		tdAPI->get()->ReqSettlementInfoConfirm(&reqsettle, 0);

		CThostFtdcQryInvestorPositionField req{};
		tdAPI->get()->ReqQryInvestorPosition(&req, -1);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return ret;
}
