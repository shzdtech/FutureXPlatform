/***********************************************************************
 * Module:  GlobalProcessorRegistry.cpp
 * Author:  milk
 * Modified: 2015年8月23日 9:19:00
 * Purpose: Implementation of the class GlobalProcessorRegistry
 ***********************************************************************/

#include "GlobalProcessorRegistry.h"
#include <mutex>

static std::vector<IProcessorBase_Ptr> registry;

size_t GlobalProcessorRegistry::RegisterProcessor(IProcessorBase_Ptr proc_ptr)
{
	static std::mutex lockobj;
	std::lock_guard<std::mutex> guard(lockobj);
	registry.push_back(proc_ptr);

	return registry.size() - 1;
}

IProcessorBase_Ptr GlobalProcessorRegistry::FindProcessor(size_t id)
{
	IProcessorBase_Ptr ret;
	if (id < registry.size())
		ret = registry[id];

	return ret;
}