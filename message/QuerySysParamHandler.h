/***********************************************************************
* Module:  QuerySysParamHandler.h
* Author:  milk
* Modified: 2017Äê07ÔÂ23ÈÕ 14:18:04
* Purpose: Declaration of the class EchoMessageHandler
***********************************************************************/

#if !defined(__message_QuerySysParamHandler_h)
#define __message_QuerySysParamHandler_h

#include "NopHandler.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT QuerySysParamHandler : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

};

#endif

