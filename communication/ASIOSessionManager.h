/***********************************************************************
 * Module:  ASIOSessionManager.h
 * Author:  milk
 * Modified: 2015年11月14日 12:07:36
 * Purpose: Declaration of the class ASIOSessionManager
 ***********************************************************************/

#if !defined(__communication_ASIOSessionManager_h)
#define __communication_ASIOSessionManager_h

#include "../message/ServerSessionManager.h"
#include "ASIOTCPSession.h"
#include <boost/asio/ip/tcp.hpp>

class ASIOSessionManager : public ServerSessionManager
{
public:
	ASIOSessionManager(IMessageServer* server);
	~ASIOSessionManager();
	asiotcpsession_ptr CreateSession(boost::asio::ip::tcp::socket&& socket);

protected:
private:

};

#endif