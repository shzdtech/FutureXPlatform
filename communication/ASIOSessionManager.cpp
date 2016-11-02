/***********************************************************************
 * Module:  ASIOSessionManager.cpp
 * Author:  milk
 * Modified: 2015年11月14日 12:07:36
 * Purpose: Implementation of the class ASIOSessionManager
 ***********************************************************************/

#include "ASIOSessionManager.h"

ASIOSessionManager::ASIOSessionManager(IMessageServer* server)
	: ServerSessionManager(server)
{

}

ASIOSessionManager::~ASIOSessionManager()
{

}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOSessionManager::CreateSession(tcp::socket&& socket socket)
// Purpose:    Implementation of ASIOSessionManager::CreateSession()
// Parameters:
// - socket
// Return:     void
////////////////////////////////////////////////////////////////////////

ASIOTCPSession_Ptr ASIOSessionManager::CreateSession(boost::asio::ip::tcp::socket&& socket,	uint max_msg_size, uint time_out)
{
	auto asioSession_Ptr = std::make_shared<ASIOTCPSession>(shared_from_this(), std::move(socket));
	asioSession_Ptr->setMaxMessageSize(MAX_MSG_SIZE);
	asioSession_Ptr->setTimeout(time_out);
	AssembleSession(asioSession_Ptr);
	
	return asioSession_Ptr;
}