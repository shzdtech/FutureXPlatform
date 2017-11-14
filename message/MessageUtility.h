#if !defined(__message_MessageUtility_h)
#define __message_MessageUtility_h

#include <memory>
#include "GlobalProcessorRegistry.h"
#include "IMessageServiceLocator.h"

class MessageUtility
{
public:
	template <typename T>
	static std::shared_ptr<T> FindGlobalProcessor(const std::string& id)
	{
		return std::static_pointer_cast<T>(GlobalProcessorRegistry::FindProcessor(id));
	}

	template <typename T>
	static T* WorkerProcessorPtr(IMessageProcessor* pMsgProcessor)
	{
		return (T*)(pMsgProcessor->getServerContext()->getSubTypeWorkerPtr());
	}

	template <typename T>
	static T* WorkerProcessorPtr(const IMessageProcessor_Ptr& msgProcessor)
	{
		return (T*)(msgProcessor->getServerContext()->getSubTypeWorkerPtr());
	}

	template <typename T>
	static T* AbstractWorkerProcessorPtr(IMessageProcessor* pMsgProcessor)
	{
		return (T*)(pMsgProcessor->getServerContext()->getAbstractSubTypeWorkerPtr());
	}

	template <typename T>
	static T* AbstractWorkerProcessorPtr(const IMessageProcessor_Ptr& msgProcessor)
	{
		return (T*)(msgProcessor->getServerContext()->getAbstractSubTypeWorkerPtr());
	}
};

#endif
