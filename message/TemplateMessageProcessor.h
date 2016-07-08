/***********************************************************************
 * Module:  TemplateMessageProcessor.h
 * Author:  milk
 * Modified: 2014年10月6日 12:14:51
 * Purpose: Declaration of the class TemplateMessageProcessor
 ***********************************************************************/

#if !defined(__message_TemplateMessageProcessor_h)
#define __message_TemplateMessageProcessor_h

#include "message_exp.h"
#include "MessageProcessor.h"
#include "ISession.h"
#include "../dataobject/data_buffer.h"
#include "BizError.h"
#include "../dataobject/BizErrorDO.h"

class MESSAGE_CLASS_EXPORT TemplateMessageProcessor : public MessageProcessor, public std::enable_shared_from_this < TemplateMessageProcessor >
{
public:
	virtual void ProcessRequest(const uint msgId, const dataobj_ptr reqDO, bool sendRsp);
	virtual void ProcessResponse(const uint msgId, const uint serialId, param_vector& rawRespParamsconst, bool sendRsp);
	virtual int SendDataObject(ISession* pSession,
		const uint msgId, const dataobj_ptr dataobj);

	int OnRecvMsg(const uint msgId, const data_buffer& msg);
	int OnResponse(const uint msgId, const uint serailId, param_vector& rawRespParams);
	void SendErrorMsg(const uint msgId, BizError& bizError, uint serialId);

protected:

private:

};

#endif