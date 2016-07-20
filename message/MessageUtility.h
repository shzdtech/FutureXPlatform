#if !defined(__message_MessageUtility_h)
#define __message_MessageUtility_h

#include <memory>
#include "GlobalProcessorRegistry.h"

class MessageUtility
{
public:
	template <typename T>
	static std::shared_ptr<T> FindGlobalProcessor(const std::string& id)
	{
		return std::static_pointer_cast<T>(GlobalProcessorRegistry::FindProcessor(id));
	}
};

#endif
