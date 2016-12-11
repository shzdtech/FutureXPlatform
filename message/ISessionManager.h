/***********************************************************************
 * Module:  ISessionManger.h
 * Author:  milk
 * Modified: 2014年10月4日 0:41:58
 * Purpose: Declaration of the class ISessionManger
 ***********************************************************************/

#if !defined(__message_ISessionManger_h)
#define __message_ISessionManger_h

#include "IMessageSession.h"
#include <set>

class ISessionManager
{
public:
	virtual void OnServerStarting(void) = 0;
	virtual void OnServerClosing(void) = 0;
	virtual void AddSession(const IMessageSession_Ptr& sessionPtr) = 0;
	virtual bool CloseSession(const IMessageSession_Ptr& sessionPtr) = 0;

protected:
private:

};

typedef std::shared_ptr<ISessionManager> ISessionManager_Ptr;

#endif