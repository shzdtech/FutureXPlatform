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
   return _uri;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageServer::RegisterServiceFactory(IMessageServiceFactory_Ptr msgSvcFactory)
// Purpose:    Implementation of MessageServer::RegisterServiceFactory()
// Parameters:
// - msgSvcFactory
// Return:     void
////////////////////////////////////////////////////////////////////////

void MessageServer::RegisterServiceFactory(IMessageServiceFactory_Ptr msgSvcFactory)
{
	_svcfactory_ptr = msgSvcFactory;
	msgSvcFactory->SetServerContext(getContext());
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageServer::GetServiceFactory()
// Purpose:    Implementation of MessageServer::GetServiceFactory()
// Return:     IMessageServiceFactory_Ptr
////////////////////////////////////////////////////////////////////////

IMessageServiceFactory_Ptr MessageServer::GetServiceFactory(void)
{
	return _svcfactory_ptr;
}

IServerContext * MessageServer::getContext()
{
	return &_context;
}
