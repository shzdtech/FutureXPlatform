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
#include "MessageException.h"
#include "../dataobject/MessageExceptionDO.h"

class MESSAGE_CLASS_EXPORT TemplateMessageProcessor : public MessageProcessor
{
public:
	virtual dataobj_ptr ProcessRequest(const uint msgId, const dataobj_ptr& reqDO, bool sendRsp);
	virtual dataobj_ptr ProcessResponse(const uint msgId, const uint serialId, param_vector& rawRespParamsconst, bool sendRsp);
	virtual int SendDataObject(ISession* pSession, const uint msgId, const uint serialId, const dataobj_ptr& dataobj);

	int OnRequest(const uint msgId, const data_buffer& msg);
	int OnResponse(const uint msgId, const uint serailId, param_vector& rawRespParams);
	void SendExceptionMessage(const uint msgId, MessageException& msgException, uint serialId);

protected:

private:

};

#endif