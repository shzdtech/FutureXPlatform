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

#include "ASIOTCPConsts.h"

class ASIOSessionManager : public ServerSessionManager
{
public:
	ASIOSessionManager(IMessageServer* server);
	~ASIOSessionManager();
	ASIOTCPSession_Ptr CreateSession(boost::asio::ip::tcp::socket&& socket, 
		uint max_msg_size = MAX_MSG_SIZE, uint time_out = SESSION_TIMEOUT);

protected:
private:

};

#endif