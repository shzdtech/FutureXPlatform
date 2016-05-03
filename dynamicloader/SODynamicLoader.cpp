/***********************************************************************
 * Module:  SODynamicLoader.cpp
 * Author:  milk
 * Modified: 2016年5月4日 1:13:16
 * Purpose: Implementation of the class SODynamicLoader
 ***********************************************************************/

#include "SODynamicLoader.h"

#if !(defined(_WIN32) || defined(_WINDOWS))

#include <dlfcn.h>

////////////////////////////////////////////////////////////////////////
// Name:       SODynamicLoader::LoadModule(const std::string& modulePath)
// Purpose:    Implementation of SODynamicLoader::LoadModule()
// Parameters:
// - modulePath
// Return:     void*
////////////////////////////////////////////////////////////////////////

void* SODynamicLoader::LoadModule(const std::string& modulePath)
{
	return dlopen(modulePath.data(), RTLD_LAZY);
}

////////////////////////////////////////////////////////////////////////
// Name:       SODynamicLoader::UnloadModule(void* module)
// Purpose:    Implementation of SODynamicLoader::UnloadModule()
// Parameters:
// - module
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool SODynamicLoader::UnloadModule(const void* module)
{
	return 0 == dlclose(module);
}

////////////////////////////////////////////////////////////////////////
// Name:       SODynamicLoader::FindFunction(const void* handle, const std::string& funcName)
// Purpose:    Implementation of SODynamicLoader::FindFunction()
// Parameters:
// - handle
// - funcName
// Return:     void*
////////////////////////////////////////////////////////////////////////

void* SODynamicLoader::FindFunction(const void* module, const std::string& funcName)
{
	return (void*)dlsym(module, funcName.data());
}

#endif