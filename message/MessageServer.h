/***********************************************************************
 * Module:  MessageServer.h
 * Author:  milk
 * Modified: 2014年10月4日 0:41:58
 * Purpose: Declaration of the class MessageServer
 ***********************************************************************/

#if !defined(__message_MessageServer_h)
#define __message_MessageServer_h

#include "IMessageServer.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT MessageServer : public IMessageServer
{
public:
	virtual bool Initialize(const std::string& uri, const std::string& config) = 0;
	virtual bool Stop(void) = 0;
	virtual bool Start(void) = 0;
	virtual bool Stopped(void) = 0;
	std::string getUri(void);
	void RegisterServiceFactory(IMessageServiceFactory_Ptr msgSvcFactory);
	IMessageServiceFactory_Ptr GetServiceFactory(void);

protected:
	std::string _uri;
	ISessionManager_Ptr _manager_ptr;
	IMessageServiceFactory_Ptr _svcfactory_ptr;

private:

};

#endif