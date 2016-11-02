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


dataobj_ptr TemplateMessageProcessor::ProcessRequest(const uint32_t msgId, const uint32_t serialId, const dataobj_ptr& reqDO, bool sendRsp)
{
	try
	{
		if (_svc_locator_ptr)
		{
			if (auto msgHandler = _svc_locator_ptr->FindMessageHandler(msgId))
			{
				if (auto session_ptr = LockMessageSession())
				{
					if (auto dataobj_ptr = msgHandler->HandleRequest(serialId, reqDO, getRawAPI(), session_ptr.get()))
					{
						if (sendRsp)
						{
							SendDataObject(session_ptr.get(), msgId, serialId, dataobj_ptr);
						}
						return dataobj_ptr;
					}
				}
			}
		}
	}
	catch (MessageException& msgEx) {
		SendExceptionMessage(msgId, msgEx, reqDO ? serialId : 0);
	}
	catch (std::exception& ex) {
		LOG_ERROR << __FUNCTION__ << ": MsgId: " << msgId << ", Error: " << ex.what();
	}
	catch (...) {
		LOG_ERROR << __FUNCTION__ << ": Unknown error occured!";
	}
	return nullptr;
}

dataobj_ptr TemplateMessageProcessor::ProcessResponse(const uint32_t msgId, const uint32_t serialId, param_vector & rawRespParams, bool sendRsp)
{
	try
	{
		if (_svc_locator_ptr)
		{
			if (auto msgHandler = _svc_locator_ptr->FindMessageHandler(msgId))
			{
				if (auto session_ptr = LockMessageSession())
				{
					if (auto dataobj_ptr = msgHandler->HandleResponse(serialId, rawRespParams, getRawAPI(), session_ptr.get()))
					{
						if (sendRsp)
						{
							SendDataObject(session_ptr.get(), msgId, serialId, dataobj_ptr);
						}
						return dataobj_ptr;
					}
				}
			}
		}
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
	return nullptr;
}

int TemplateMessageProcessor::SendDataObject(ISession* session,
	const uint32_t msgId, const uint32_t serialId, const dataobj_ptr& dataobj)
{
	int ret = 0;
	if (_svc_locator_ptr)
	{
		if (auto msgSerilzer = _svc_locator_ptr->FindDataSerializer(msgId))
		{
			dataobj->SerialId = serialId;
			if (data_buffer db = msgSerilzer->Serialize(dataobj))
				ret = session->WriteMessage(msgId, db);
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       TemplateMessageProcessor::OnRequest(const uint32_t msgId, const data_buffer msg)
// Purpose:    Implementation of TemplateMessageProcessor::OnRequest()
// Parameters:
// - msgId
// - msg
// Return:     int
////////////////////////////////////////////////////////////////////////

int TemplateMessageProcessor::OnRequest(const uint32_t msgId, const data_buffer& msg)
{
	if (_svc_locator_ptr)
		if (auto msgSerilzer = _svc_locator_ptr->FindDataSerializer(msgId))
			if (auto reqDO = msgSerilzer->Deserialize(msg))
				ProcessRequest(msgId, reqDO->SerialId, reqDO, true);

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

int TemplateMessageProcessor::OnResponse(const uint32_t msgId, const uint32_t serialId, param_vector& rawRespParams)
{
	ProcessResponse(msgId, serialId, rawRespParams, true);
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Name:       TemplateMessageProcessor::SendExceptionMessage(BizError& bizError)
// Purpose:    Implementation of TemplateMessageProcessor::SendExceptionMessage()
// Parameters:
// - bizError
// Return:     void
////////////////////////////////////////////////////////////////////////

void TemplateMessageProcessor::SendExceptionMessage(uint32_t msgId, MessageException& msgException, uint32_t serialId)
{
	dataobj_ptr dataobj(new MessageExceptionDO(msgId, serialId, msgException.ErrorType(), msgException.ErrorCode(),
		msgException.what()));
	if (auto exMsg = ExceptionSerializer::Instance()->Serialize(dataobj))
		if (auto session_ptr = LockMessageSession())
			session_ptr->WriteMessage(MSG_ID_ERROR, exMsg);
}