/***********************************************************************
 * Module:  MessageServer.cpp
 * Author:  milk
 * Modified: 2014年10月10日 13:07:37
 * Purpose: Implementation of the class MessageServer
 ***********************************************************************/

#include "MessageServer.h"

////////////////////////////////////////////////////////////////////////
// Name:       MessageServer::getUri()
// Purpose:    Implementation of MessageServer::getUri()
// Return:     std::string
////////////////////////////////////////////////////////////////////////

const std::string& MessageServer::getUri(void)
{
   return _context.getServerUri();
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageServer::RegisterServiceFactory(const IMessageServiceFactory_Ptr& msgSvcFactory)
// Purpose:    Implementation of MessageServer::RegisterServiceFactory()
// Parameters:
// - msgSvcFactory
// Return:     void
////////////////////////////////////////////////////////////////////////

void MessageServer::RegisterServiceFactory(const IMessageServiceFactory_Ptr& msgSvcFactory)
{
	_svcfactory_ptr = msgSvcFactory;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageServer::getServiceFactory()
// Purpose:    Implementation of MessageServer::getServiceFactory()
// Return:     IMessageServiceFactory_Ptr
////////////////////////////////////////////////////////////////////////

IMessageServiceFactory_Ptr& MessageServer::getServiceFactory(void)
{
	return _svcfactory_ptr;
}

IServerContext * MessageServer::getContext()
{
	return &_context;
}
