/***********************************************************************
 * Module:  ServerSessionManager.h
 * Author:  milk
 * Modified: 2015年10月22日 21:48:47
 * Purpose: Declaration of the class ServerSessionManager
 ***********************************************************************/

#if !defined(__message_ServerSessionManager_h)
#define __message_ServerSessionManager_h

#include "SessionManager.h"
#include "IMessageServiceLocator.h"
#include "IMessageServiceFactory.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT ServerSessionManager : public SessionManager
{
public:
	ServerSessionManager(IMessageServer* server);
	~ServerSessionManager();

	void OnServerStarting(void);
	void OnServerClosing(void);

protected:
	void AssembleSession(IMessageSession_Ptr msgSessionPtr);
	IMessageServiceLocator_Ptr _msgsvclocator;

	void OnSessionClosing(IMessageSession_Ptr msgSessionPtr);
private:

};

#endif