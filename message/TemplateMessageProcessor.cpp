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


dataobj_ptr TemplateMessageProcessor::HandleRequest(const uint msgId, const dataobj_ptr reqDO, const bool sendRsp)
{
	dataobj_ptr ret;

	try
	{
		if (_svc_locator_ptr)
		{
			if (auto msgHandler = _svc_locator_ptr->FindMessageHandler(msgId))
			{
				if (auto pMsgSession = getSession())
				{
					if (ret = msgHandler->HandleRequest(reqDO, getRawAPI(), pMsgSession))
					{
						if (sendRsp)
						{
							auto dataSerilzer = _svc_locator_ptr->FindDataSerializer(msgId);
							if (dataSerilzer) {
								data_buffer db = dataSerilzer->Serialize(ret);
								pMsgSession->WriteMessage(msgId, db);
							}
						}

					}
				}
			}
		}
	}
	catch (BizError& bizErr) {
		SendErrorMsg(msgId, bizErr);
	}
	catch (std::exception& ex) {
		LOG(ERROR) << __FUNCTION__ << " MsgId: " << msgId << ", Error: " << ex.what() << std::endl;
	}
	catch (...) {
		LOG(ERROR) << __FUNCTION__ << " unknown error occured." << std::endl;
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

int TemplateMessageProcessor::OnRecvMsg(const uint msgId, const data_buffer& msg) {
	try
	{
		if (_svc_locator_ptr)
		{
			auto msgSerilzer = _svc_locator_ptr->FindDataSerializer(msgId);
			if (msgSerilzer)
				if (auto reqDO = msgSerilzer->Deserialize(msg))
					HandleRequest(msgId, reqDO, true);
		}
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

int TemplateMessageProcessor::OnResponse(const uint msgId, param_vector& rawRespParams) {
	try {
		if (_svc_locator_ptr)
		{
			if (auto msgHandler = _svc_locator_ptr->FindMessageHandler(msgId))
			{
				if (auto pMsgSession = getSession())
				{
					if (auto dataobj = msgHandler->HandleResponse(rawRespParams, getRawAPI(), pMsgSession))
					{
						if (auto msgSerilzer = _svc_locator_ptr->FindDataSerializer(msgId))
						{
							data_buffer db = msgSerilzer->Serialize(dataobj);
							pMsgSession->WriteMessage(msgId, db);
						}
					}
				}
			}
		}
	}
	catch (BizError& bizErr) {
		SendErrorMsg(msgId, bizErr);
	}
	catch (std::exception& ex) {
		LOG(ERROR) << __FUNCTION__ << " MsgId: " << msgId << ", Error: " << ex.what() << std::endl;
	}
	catch (...) {
		LOG(ERROR) << __FUNCTION__  << " unknown error occured." << std::endl;
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

void TemplateMessageProcessor::SendErrorMsg(uint msgId, BizError& bizError)
{
	dataobj_ptr dataobj(new BizErrorDO
		(msgId, bizError.ErrorCode(), bizError.what(), bizError.SysErrCode(), bizError.SerialId()));
	data_buffer msg = BizErrorSerializer::Instance()->Serialize(dataobj);
	if (auto session_ptr = getSession())
		session_ptr->WriteMessage(MSG_ID_ERROR, msg);
}