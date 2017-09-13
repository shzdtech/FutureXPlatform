/***********************************************************************
 * Module:  IMessageProcessor.h
 * Author:  milk
 * Modified: 2014年10月6日 12:49:50
 * Purpose: Declaration of the class IMessageProcessor
 ***********************************************************************/

#if !defined(__message_IMessageProcessor_h)
#define __message_IMessageProcessor_h

#include "IProcessorBase.h"
#include "IMessageSession.h"
#include "IRawAPI.h"
#include "../dataobject/dataobjectbase.h"
#include "../common/typedefs.h"

class IMessageProcessor : public IProcessorBase
{
public:
	virtual void setMessageSession(const IMessageSession_Ptr& msgSession_ptr) = 0;
	virtual void setServiceLocator(const IMessageServiceLocator_Ptr& svc_locator_ptr) = 0;
	virtual void setServerContext(IServerContext* serverCtx) = 0;
	virtual bool OnSessionClosing(void) = 0;
	virtual IRawAPI* getRawAPI(void) = 0;
	virtual IMessageSession_Ptr& getMessageSession(void) = 0;

	virtual dataobj_ptr ProcessRequest(const uint32_t msgId, const uint32_t serialId, const dataobj_ptr& reqDO, bool sendRsp) = 0;
	virtual dataobj_ptr ProcessResponse(const uint32_t msgId, const uint32_t serialId, param_vector& rawRespParamsconst, bool sendRsp) = 0;

protected:
private:

};

#endif