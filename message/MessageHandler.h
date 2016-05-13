/***********************************************************************
 * Module:  MessageHandler.h
 * Author:  milk
 * Modified: 2014年10月3日 22:00:52
 * Purpose: Declaration of the class MessageHandler
 ***********************************************************************/

#if !defined(__message_MessageHandler_h)
#define __message_MessageHandler_h

#include <atomic>
#include "IMessageHandler.h"
#include "ISession.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT MessageHandler : public IMessageHandler
{
public:
	virtual dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session) = 0;
	virtual dataobj_ptr HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session) = 0;

	virtual bool CheckLogin(ISession* session, bool throwBizErr = true);

protected:
	static std::atomic_uint _requestIdGen;

private:


};

#endif