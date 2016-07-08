/***********************************************************************
 * Module:  TemplateMessageProcessor.cpp
 * Author:  milk
 * Modified: 2014年10月6日 12:14:51
 * Purpose: Implementation of the class TemplateMessageProcessor
 ***********************************************************************/

#include <iostream>
#include <glog/logging.h>
#include "DefMessageID.h"
#include "message_macro.h"
#include "TemplateMessageProcessor.h"
#include "../dataobject/BizErrorSerializer.h"


void TemplateMessageProcessor::ProcessRequest(const uint msgId, const dataobj_ptr reqDO, bool sendRsp)
{
	if (_svc_locator_ptr)
	{
		if (auto msgHandler = _svc_locator_ptr->FindMessageHandler(msgId))
		{
			if (auto pMsgSession = getSession())
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
			if (auto pMsgSession = getSession())
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
	const uint msgId, const dataobj_ptr dataobj)
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
// Name:       TemplateMessageProcessor::OnRecvMsg(const uint msgId, const data_buffer msg)
// Purpose:    Implementation of TemplateMessageProcessor::OnRecvMsg()
// Parameters:
// - msgId
// - msg
// Return:     int
////////////////////////////////////////////////////////////////////////

int TemplateMessageProcessor::OnRecvMsg(const uint msgId, const data_buffer& msg)
{
	dataobj_ptr reqDO;
	try
	{
		if (_svc_locator_ptr)
		{
			auto msgSerilzer = _svc_locator_ptr->FindDataSerializer(msgId);
			if (msgSerilzer)
				if (reqDO = msgSerilzer->Deserialize(msg))
					ProcessRequest(msgId, reqDO, true);
		}
	}
	catch (BizError& bizErr) {
		SendErrorMsg(msgId, bizErr, reqDO ? reqDO->SerialId : 0);
	}
	catch (std::exception& ex) {
		LOG(ERROR) << __FUNCTION__ << " MsgId: " << msgId << ", Error: " << ex.what() << std::endl;
	}
	catch (...) {
		LOG(ERROR) << "OnRecvMsg unknown error occured." << std::endl;
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
	catch (BizError& bizErr) {
		SendErrorMsg(msgId, bizErr, serialId);
	}
	catch (std::exception& ex) {
		LOG(ERROR) << __FUNCTION__ << " MsgId: " << msgId << ", Error: " << ex.what() << std::endl;
	}
	catch (...) {
		LOG(ERROR) << __FUNCTION__ << " unknown error occured." << std::endl;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////
// Name:       TemplateMessageProcessor::SendErrorMsg(BizError& bizError)
// Purpose:    Implementation of TemplateMessageProcessor::SendErrorMsg()
// Parameters:
// - bizError
// Return:     void
////////////////////////////////////////////////////////////////////////

void TemplateMessageProcessor::SendErrorMsg(uint msgId, BizError& bizError, uint serialId)
{
	dataobj_ptr dataobj(new BizErrorDO
	(msgId, bizError.ErrorCode(), bizError.what(), bizError.SysErrCode(), serialId));
	data_buffer msg = BizErrorSerializer::Instance()->Serialize(dataobj);
	if (auto session_ptr = getSession())
		session_ptr->WriteMessage(MSG_ID_ERROR, msg);
}