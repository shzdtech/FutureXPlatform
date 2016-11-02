#if !defined(__message_MessageInterfaceDeclare_h)
#define __message_MessageInterfaceDeclare_h

#include <memory>

class ISession;
class IMessageSession;
class IMessageSessionEvent;
class IProcessorBase;
class IMessageProcessor;
class IMessageServiceLocator;
class IMessageHandler;
class ISessionManager;

typedef std::shared_ptr<IProcessorBase> IProcessorBase_Ptr;
typedef std::shared_ptr<IMessageProcessor> IMessageProcessor_Ptr;
typedef std::weak_ptr<IMessageProcessor> IMessageProcessor_WkPtr;

typedef std::shared_ptr<ISession> ISession_Ptr;
typedef std::shared_ptr<IMessageSession> IMessageSession_Ptr;
typedef std::weak_ptr<IMessageSession> IMessageSession_WkPtr;
typedef std::weak_ptr<IMessageSessionEvent> IMessageSessionEvent_WkPtr;

typedef std::shared_ptr<IMessageServiceLocator> IMessageServiceLocator_Ptr;
typedef std::shared_ptr<ISessionManager> ISessionManager_Ptr;

typedef std::shared_ptr<IMessageHandler> IMessageHandler_Ptr;

#endif
