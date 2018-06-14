/***********************************************************************
 * Module:  CTPOTCTradeLoginHandler.cpp
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Implementation of the class CTPOTCTradeLoginHandler
 ***********************************************************************/

#include "CTPOTCTradeLoginHandler.h"
#include "XTOTCTradeWorkerProcessor.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../utility/TUtil.h"
#include "../common/Attribute_Key.h"
#include "../dataobject/TypedefDO.h"
#include "../litelogger/LiteLogger.h"


CTPTradeWorkerProcessor* CTPOTCTradeLoginHandler::GetWorkerProcessor(const IMessageProcessor_Ptr& msgProcessor)
{
	return MessageUtility::WorkerProcessorPtr<XTOTCTradeWorkerProcessor>(msgProcessor);
}
