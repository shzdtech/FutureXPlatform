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
	static T* ServerWorkerProcessor(const IProcessorBase_Ptr& processorPtr)
	{
		return (T*)(processorPtr->getServerContext()->getSubTypeWorkerPtr());
	}
};

#endif
