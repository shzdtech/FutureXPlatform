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
		auto thisptr = shared_from_this();
		if (_svc_locator_ptr)
		{
			if (auto msgHandler = _svc_locator_ptr->FindMessageHandler(msgId))
			{
				if (auto dataobj_ptr = msgHandler->HandleRequest(serialId, reqDO, getRawAPI(), thisptr, getMessageSession()))
				{
					if (sendRsp)
					{
						SendDataObject(getMessageSession(), msgId, serialId, dataobj_ptr);
					}
					return dataobj_ptr;
				}
			}
		}
	}
	catch (MessageException& msgEx) {
		SendExceptionMessage(getMessageSession(), msgId, reqDO ? serialId:0, msgEx);
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
		auto thisptr = shared_from_this();
		if (_svc_locator_ptr)
		{
			if (auto msgHandler = _svc_locator_ptr->FindMessageHandler(msgId))
			{
				if (auto dataobj_ptr = msgHandler->HandleResponse(serialId, rawRespParams, getRawAPI(), thisptr, getMessageSession()))
				{
					if (sendRsp)
					{
						SendDataObject(getMessageSession(), msgId, serialId, dataobj_ptr);
					}
					return dataobj_ptr;
				}
			}
		}
	}
	catch (MessageException& msgEx) {
		SendExceptionMessage(getMessageSession(), msgId, serialId, msgEx);
	}
	catch (std::exception& ex) {
		LOG_ERROR << __FUNCTION__ << ": MsgId: " << msgId << ", Error: " << ex.what();
	}
	catch (...) {
		LOG_ERROR << __FUNCTION__ << ": Unknown error occured!";
	}
	return nullptr;
}

int TemplateMessageProcessor::SendDataObject(const ISession_Ptr& session,
	const uint32_t msgId, const uint32_t serialId, const dataobj_ptr& dataobj)
{
	int ret = 0;
	if (auto db = Deserialize(msgId, serialId, dataobj))
	{
		if (session)
			ret = session->WriteMessage(msgId, db);
	}
	return ret;
}

int TemplateMessageProcessor::SendDataObject(const ISession_Ptr & session, const uint32_t msgId, const data_buffer & msg)
{
	int ret = 0;
	if (session)
		ret = session->WriteMessage(msgId, msg);
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


void TemplateMessageProcessor::SendExceptionMessage(const ISession_Ptr& session, uint32_t msgId, uint32_t serialId, const MessageException& msgException)
{
	dataobj_ptr dataobj(new MessageExceptionDO(msgId, serialId, msgException.ErrorType(), msgException.ErrorCode(), msgException.what()));
	if (auto exMsg = ExceptionSerializer::Instance()->Serialize(dataobj))
		session->WriteMessage(MSG_ID_ERROR, exMsg);
}

data_buffer TemplateMessageProcessor::Deserialize(const uint32_t msgId, const uint32_t serialId, const dataobj_ptr & dataobj)
{
	data_buffer db;
	if (_svc_locator_ptr)
	{
		if (auto msgSerilzer = _svc_locator_ptr->FindDataSerializer(msgId))
		{
			dataobj->SerialId = serialId;
			db = msgSerilzer->Serialize(dataobj);
		}
	}
	return db;
}

data_buffer TemplateMessageProcessor::DeserializeException(uint32_t msgId, uint32_t serialId, const MessageException& msgException)
{
	dataobj_ptr dataobj(new MessageExceptionDO(msgId, serialId, msgException.ErrorType(), msgException.ErrorCode(), msgException.what()));
	return ExceptionSerializer::Instance()->Serialize(dataobj);
}