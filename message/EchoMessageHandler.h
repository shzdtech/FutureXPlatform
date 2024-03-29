/***********************************************************************
 * Module:  EchoMessageHandler.h
 * Author:  milk
 * Modified: 2014年10月12日 14:18:04
 * Purpose: Declaration of the class EchoMessageHandler
 ***********************************************************************/

#if !defined(__message_EchoMessageHandler_h)
#define __message_EchoMessageHandler_h

#include "NopHandler.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT EchoMessageHandler : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif