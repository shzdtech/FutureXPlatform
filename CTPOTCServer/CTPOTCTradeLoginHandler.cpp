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


bool CTPOTCTradeLoginHandler::LoginFromDB(const IMessageProcessor_Ptr& msgProcessor)
{
	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(msgProcessor))
	{
		auto session = msgProcessor->getMessageSession();

		pWorkerProc->RegisterLoggedSession(session);

		if (pWorkerProc->IsLoadPositionFromDB())
		{
			pWorkerProc->LoadPositonFromDatabase(session->getUserInfo().getUserId(),
				session->getUserInfo().getInvestorId(), std::to_string(session->getUserInfo().getTradingDay()));

			((CTPProcessor*)msgProcessor.get())->DataLoadMask |= CTPProcessor::POSITION_DATA_LOADED;
		}
	}
}
