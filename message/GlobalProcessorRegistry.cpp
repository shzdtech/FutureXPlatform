/***********************************************************************
 * Module:  GlobalProcessorRegistry.cpp
 * Author:  milk
 * Modified: 2015年8月23日 9:19:00
 * Purpose: Implementation of the class GlobalProcessorRegistry
 ***********************************************************************/

#include "GlobalProcessorRegistry.h"
#include <map>
#include <mutex>

static std::map<std::string, IMessageProcessor_Ptr> registry;

void GlobalProcessorRegistry::RegisterProcessor(const std::string& processName, const IMessageProcessor_Ptr& proc_ptr)
{
	registry[processName] = proc_ptr;
}

IMessageProcessor_Ptr GlobalProcessorRegistry::FindProcessor(const std::string& processName)
{
	IMessageProcessor_Ptr ret;

	auto it = registry.find(processName);
	if (it != registry.end())
		ret = it->second;

	return ret;
}