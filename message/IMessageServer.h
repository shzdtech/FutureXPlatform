/***********************************************************************
 * Module:  IMessageServer.h
 * Author:  milk
 * Modified: 2014年10月4日 0:37:07
 * Purpose: Declaration of the class IMessageServer
 ***********************************************************************/

#if !defined(__message_IMessageServer_h)
#define __message_IMessageServer_h

#include "ISessionManager.h"
#include "IMessageServiceFactory.h"

class IMessageServer
{
public:
   virtual bool Initialize(const std::string& uri, const std::string& config, const std::string& section)=0;
   virtual bool Start(void)=0;
   virtual bool Stop(void)=0;
   virtual bool Stopped(void)=0;
   virtual const std::string& getUri(void)=0;
   virtual void RegisterServiceFactory(const IMessageServiceFactory_Ptr& msgSvcFactory) = 0;
   virtual IMessageServiceFactory_Ptr& getServiceFactory(void) = 0;
   virtual IServerContext* getContext() = 0;

protected:
private:

};

typedef std::shared_ptr<IMessageServer> IMessageServer_Ptr;
#endif