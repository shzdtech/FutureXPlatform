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
#include "IProcessorBase.h"
#include "MessageInterfaceDeclare.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT MessageHandler : public IMessageHandler
{
public:
	virtual bool CheckLogin(const IMessageSession_Ptr& session, bool throwBizErr = true);

protected:
	static std::atomic_uint _requestIdGen;

private:


};

#endif