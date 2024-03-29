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
	virtual dataobj_ptr ProcessRequest(const uint32_t msgId, const uint32_t serialId, const dataobj_ptr& reqDO, bool sendRsp);
	virtual dataobj_ptr ProcessResponse(const uint32_t msgId, const uint32_t serialId, param_vector& rawRespParamsconst, bool sendRsp);
	virtual data_buffer Deserialize(const uint32_t msgId, const uint32_t serialId, const dataobj_ptr& dataobj);
	virtual data_buffer DeserializeException(uint32_t msgId, uint32_t serialId, const MessageException & msgException);
	virtual int SendDataObject(const ISession_Ptr& session, const uint32_t msgId, const uint32_t serialId, const dataobj_ptr& dataobj);
	virtual int SendDataObject(const ISession_Ptr& session, const uint32_t msgId, const data_buffer& msg);
	virtual void SendExceptionMessage(const ISession_Ptr& session, const uint32_t msgId, uint32_t serialId, const MessageException& msgException);

	int OnRequest(const uint32_t msgId, const data_buffer& msg);
	int OnResponse(const uint32_t msgId, const uint32_t serailId, param_vector& rawRespParams);

protected:

private:

};

#endif