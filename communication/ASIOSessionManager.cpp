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

ASIOTCPSession_Ptr ASIOSessionManager::CreateSession(boost::asio::ip::tcp::socket&& socket)
{
	auto asioSession_Ptr = std::make_shared<ASIOTCPSession>(std::move(socket));
	asioSession_Ptr->addListener(shared_from_this());
	AssembleSession(asioSession_Ptr);
	
	return asioSession_Ptr;
}