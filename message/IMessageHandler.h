/***********************************************************************
 * Module:  IMessageHandler.h
 * Author:  milk
 * Modified: 2014年10月6日 13:18:32
 * Purpose: Declaration of the class IMessageHandler
 ***********************************************************************/

#if !defined(__message_IMessageHandler_h)
#define __message_IMessageHandler_h

#include "../dataobject/dataobjectbase.h"
#include "IMessageSession.h"
#include "IRawAPI.h"
#include "BizError.h"
#include "../systemsettings/SysParam.h"
#include "MessageInterfaceDeclare.h"


class IMessageHandler
{
public:
	virtual dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session) = 0;
	virtual dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session) = 0;

protected:
private:

};

#endif