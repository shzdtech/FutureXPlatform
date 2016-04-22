/***********************************************************************
 * Module:  ISession.h
 * Author:  milk
 * Modified: 2015年8月1日 15:24:50
 * Purpose: Declaration of the class ISession
 ***********************************************************************/

#if !defined(__message_ISession_h)
#define __message_ISession_h

#include "../dataobject/data_buffer.h"
#include "typedef.h"
#include "ISessionContext.h"

class IProcessorBase;
typedef std::shared_ptr<IProcessorBase> IProcessorBase_Ptr;


class ISession
{
public:
	virtual uint64_t Id() = 0;
	virtual int WriteMessage(const uint msgId, const data_buffer& msg) = 0;
	virtual int WriteMessage(const data_buffer& msg) = 0;
	virtual ISessionContext_Ptr getContext(void) = 0;
	virtual bool IsLogin(void) = 0;
	virtual void setLoginStatus(bool status) = 0;
	virtual IUserInfo_Ptr getUserInfo(void) = 0;
	virtual IProcessorBase_Ptr getProcessor(void) = 0;

	std::shared_ptr<void> Tag;
protected:
private:

};

#endif