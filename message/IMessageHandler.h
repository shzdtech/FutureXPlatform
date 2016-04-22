/***********************************************************************
 * Module:  IMessageHandler.h
 * Author:  milk
 * Modified: 2014年10月6日 13:18:32
 * Purpose: Declaration of the class IMessageHandler
 ***********************************************************************/

#if !defined(__message_IMessageHandler_h)
#define __message_IMessageHandler_h

#include "../dataobject/AbstractDataObj.h"
#include "ISession.h"
#include "IRawAPI.h"

class IMessageHandler
{
public:
	virtual dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session) = 0;
	virtual dataobj_ptr HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session) = 0;

protected:
private:

};

typedef std::shared_ptr<IMessageHandler> IMessageHandler_Ptr;
#endif