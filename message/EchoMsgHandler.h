/***********************************************************************
 * Module:  EchoMsgHandler.h
 * Author:  milk
 * Modified: 2014年10月12日 14:18:04
 * Purpose: Declaration of the class EchoMsgHandler
 ***********************************************************************/

#if !defined(__message_EchoMsgHandler_h)
#define __message_EchoMsgHandler_h

#include "NopHandler.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT EchoMsgHandler : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif