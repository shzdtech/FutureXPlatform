/***********************************************************************
 * Module:  ISession.h
 * Author:  milk
 * Modified: 2015年8月1日 15:24:50
 * Purpose: Declaration of the class ISession
 ***********************************************************************/

#if !defined(__message_ISession_h)
#define __message_ISession_h

#include "../dataobject/data_buffer.h"
#include "../common/typedefs.h"
#include "IMessageContext.h"
#include "IUserInfo.h"
#include "MessageInterfaceDeclare.h"

class ISession
{
public:
	virtual uint64_t Id() = 0;
	virtual int WriteMessage(const uint msgId, const data_buffer& msg) = 0;
	virtual int WriteMessage(const data_buffer& msg) = 0;
	virtual IMessageContext_Ptr getContext(void) = 0;
	virtual time_t getLoginTimeStamp(void) = 0;
	virtual void setLoginTimeStamp(time_t tm = 0) = 0;
	virtual void setLogout(void) = 0;
	virtual IUserInfo_Ptr getUserInfo(void) = 0;
	virtual IProcessorBase_Ptr getProcessor(void) = 0;

	std::shared_ptr<void> Tag;
protected:
private:

};

#endif