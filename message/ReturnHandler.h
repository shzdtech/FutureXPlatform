/***********************************************************************
* Module:  ReturnHandler.h
* Author:  milk
* Modified: 2017Äê01ÔÂ02ÈÕ 20:03:39
* Purpose: Declaration of the class ReturnHandler
***********************************************************************/

#if !defined(__message_ReturnHandler_h)
#define __message_ReturnHandler_h

#include "MessageHandler.h"

class ReturnHandler : public MessageHandler
{
public:
	virtual dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session) { return reqDO; };
	virtual dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
	{ return *((dataobj_ptr*)(rawRespParams[0])); }

protected:
private:

};

#endif