/***********************************************************************
 * Module:  SessionManager.h
 * Author:  milk
 * Modified: 2014年10月3日 23:10:38
 * Purpose: Declaration of the class SessionManager
 ***********************************************************************/

#if !defined(__message_SessionManager_h)
#define __message_SessionManager_h

#include "ISessionManager.h"
#include "IMessageServer.h"
#include <set>
#include <mutex>
#include "libcuckoo/cuckoohash_map.hh"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT SessionManager : public ISessionManager, public std::enable_shared_from_this<SessionManager>
{
public:
	SessionManager(IMessageServer* server);
	~SessionManager();

	void OnServerStarting(void);
	void OnServerClosing(void);
	void AddSession(const IMessageSession_Ptr& sessionPtr);
	bool CloseSession(const IMessageSession_Ptr& sessionPtr);

protected:
	cuckoohash_map<IMessageProcessor_Ptr, bool> _sessionSet;
	IMessageServer* _server;

private:

};

#endif