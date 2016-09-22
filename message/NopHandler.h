/***********************************************************************
 * Module:  NopHandler.h
 * Author:  milk
 * Modified: 2014年10月17日 20:03:39
 * Purpose: Declaration of the class NopHandler
 ***********************************************************************/

#if !defined(__message_NopHandler_h)
#define __message_NopHandler_h

#include "MessageHandler.h"

class MESSAGE_CLASS_EXPORT NopHandler : public MessageHandler
{
public:
	virtual dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session){ return nullptr; };
	virtual dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session){ return nullptr; }

protected:
private:

};

#endif