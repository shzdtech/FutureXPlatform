/***********************************************************************
 * Module:  AbstractDynamicLoader.cpp
 * Author:  milk
 * Modified: 2014年10月1日 12:13:03
 * Purpose: Implementation of the class AbstractDynamicLoader
 ***********************************************************************/

#include "AbstractDynamicLoader.h"
#include "WinDynamicLoader.h"
#include "SODynamicLoader.h"

const char* AbstractDynamicLoader::STR_CREATEINSTANCE = "CreateInstance";

////////////////////////////////////////////////////////////////////////
// Name:       AbstractDynamicLoader::Instance()
// Purpose:    Implementation of AbstractDynamicLoader::Instance()
// Return:     AbstractDynamicLoader*
////////////////////////////////////////////////////////////////////////

AbstractDynamicLoader* AbstractDynamicLoader::Instance(void)
{
#if defined(_WIN32) || defined(_WINDOWS)
	static WinDynamicLoader singleton;
#else
	static SODynamicLoader singleton;
#endif
	return &singleton;
}

void * AbstractDynamicLoader::LoadModule(const char* moduleUUID, const char* modulePath)
{
	void* module = nullptr;
	auto itr = uuidHandlerMap.find(moduleUUID);
	if (itr != uuidHandlerMap.end()) {
		module = itr->second;
	}
	else {
		if (module = LoadModule(modulePath))
			uuidHandlerMap[moduleUUID] = module;
	}

	return module;
}

void * AbstractDynamicLoader::FindModule(const char* moduleUUID)
{
	void* funcPtr = nullptr;
	auto iter = uuidHandlerMap.find(moduleUUID);
	if (iter != uuidHandlerMap.end())
		funcPtr = iter->second;

	return funcPtr;
}

bool AbstractDynamicLoader::UnloadModule(const char* moduleUUID)
{
	bool ret = true;

	auto iter = uuidHandlerMap.find(moduleUUID);
	if (iter != uuidHandlerMap.end())
	{
		// Thread safe
		if (uuidHandlerMap.erase(iter->first) > 0)
			ret = UnloadModule(iter->second);
	}

	return ret;
}

bool AbstractDynamicLoader::CreateInstance(const void * handle, const char* classUUID, void ** instance)
{
	*instance = nullptr;
	typedef void* (*instanceFun)(const char*);
	instanceFun instanceCall;
	instanceCall = (instanceFun)FindFunction(handle, AbstractDynamicLoader::STR_CREATEINSTANCE);
	if (instanceCall)
		*instance = instanceCall(classUUID);

	return *instance != nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       AbstractDynamicLoader::AbstractDynamicLoader()
// Purpose:    Implementation of AbstractDynamicLoader::AbstractDynamicLoader()
// Return:     
////////////////////////////////////////////////////////////////////////

AbstractDynamicLoader::AbstractDynamicLoader()
{
}