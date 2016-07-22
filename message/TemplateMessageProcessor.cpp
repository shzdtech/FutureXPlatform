/***********************************************************************
 * Module:  TemplateMessageProcessor.cpp
 * Author:  milk
 * Modified: 2014年10月6日 12:14:51
 * Purpose: Implementation of the class TemplateMessageProcessor
 ***********************************************************************/

#include <iostream>
#include "../litelogger/LiteLogger.h"
#include "DefMessageID.h"
#include "message_macro.h"
#include "TemplateMessageProcessor.h"
#include "../dataserializer/ExceptionSerializer.h"


void TemplateMessageProcessor::ProcessRequest(const uint msgId, const dataobj_ptr& reqDO, bool sendRsp)
{
	if (_svc_locator_ptr)
	{
		if (auto msgHandler = _svc_locator_ptr->FindMessageHandler(msgId))
		{
			if (auto pMsgSession = LockMessageSession().get())
			{
				if (auto dataobj_ptr = msgHandler->HandleRequest(reqDO, getRawAPI(), pMsgSession))
				{
					if (sendRsp)
					{
						SendDataObject(pMsgSession, msgId, dataobj_ptr);
					}
				}
			}
		}
	}
}

void TemplateMessageProcessor::ProcessResponse(const uint msgId, const uint serialId, param_vector & rawRespParams, bool sendRsp)
{
	if (_svc_locator_ptr)
	{
		if (auto msgHandler = _svc_locator_ptr->FindMessageHandler(msgId))
		{
			if (auto pMsgSession = LockMessageSession().get())
			{
				if (auto dataobj_ptr = msgHandler->HandleResponse(serialId, rawRespParams, getRawAPI(), pMsgSession))
				{
					if (sendRsp)
					{
						SendDataObject(pMsgSession, msgId, dataobj_ptr);
					}
				}
			}
		}
	}
}

int TemplateMessageProcessor::SendDataObject(ISession* session,
	const uint msgId, const dataobj_ptr& dataobj)
{
	int ret = 0;
	if (auto msgSerilzer = _svc_locator_ptr->FindDataSerializer(msgId))
	{
		data_buffer db = msgSerilzer->Serialize(dataobj);
		ret = session->WriteMessage(msgId, db);
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       TemplateMessageProcessor::OnRequest(const uint msgId, const data_buffer msg)
// Purpose:    Implementation of TemplateMessageProcessor::OnRequest()
// Parameters:
// - msgId
// - msg
// Return:     int
////////////////////////////////////////////////////////////////////////

int TemplateMessageProcessor::OnRequest(const uint msgId, const data_buffer& msg)
{
	dataobj_ptr reqDO;
	try
	{
		if (_svc_locator_ptr)
			if(auto msgSerilzer = _svc_locator_ptr->FindDataSerializer(msgId))
				if (reqDO = msgSerilzer->Deserialize(msg))
					ProcessRequest(msgId, reqDO, true);
	}
	catch (MessageException& msgEx) {
		SendExceptionMessage(msgId, msgEx, reqDO ? reqDO->SerialId : 0);
	}
	catch (std::exception& ex) {
		LOG_ERROR << __FUNCTION__ <<": MsgId: " << msgId << ", Error: " << ex.what();
	}
	catch (...) {
		LOG_ERROR << __FUNCTION__ << ": Unknown error occured!";
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////
// Name:       TemplateMessageProcessor::OnResponse(int msgId, dataobj_ptr absDO)
// Purpose:    Implementation of TemplateMessageProcessor::OnResponse()
// Parameters:
// - msgId
// - absDO
// Return:     int
////////////////////////////////////////////////////////////////////////

int TemplateMessageProcessor::OnResponse(const uint msgId, const uint serialId, param_vector& rawRespParams) {
	try {
		ProcessResponse(msgId, serialId, rawRespParams, true);
	}
	catch (MessageException& msgEx) {
		SendExceptionMessage(msgId, msgEx, serialId);
	}
	catch (std::exception& ex) {
		LOG_ERROR << __FUNCTION__ << ": MsgId: " << msgId << ", Error: " << ex.what();
	}
	catch (...) {
		LOG_ERROR << __FUNCTION__ << ": Unknown error occured!";
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////
// Name:       TemplateMessageProcessor::SendExceptionMessage(BizError& bizError)
// Purpose:    Implementation of TemplateMessageProcessor::SendExceptionMessage()
// Parameters:
// - bizError
// Return:     void
////////////////////////////////////////////////////////////////////////

void TemplateMessageProcessor::SendExceptionMessage(uint msgId, MessageException& msgException, uint serialId)
{
	dataobj_ptr dataobj(new MessageExceptionDO(msgId, serialId, msgException.ErrorType(), msgException.ErrorCode(),
		msgException.what()));
	data_buffer msg = ExceptionSerializer::Instance()->Serialize(dataobj);
	if (auto session_ptr = LockMessageSession())
		session_ptr->WriteMessage(MSG_ID_ERROR, msg);
}