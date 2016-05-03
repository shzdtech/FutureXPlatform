/***********************************************************************
 * Module:  WinDynamicLoader.cpp
 * Author:  milk
 * Modified: 2014年10月1日 15:52:43
 * Purpose: Implementation of the class WinDynamicLoader
 ***********************************************************************/

#include "WinDynamicLoader.h"

#if defined(_WIN32) || defined(_WINDOWS)

#include <windows.h>

////////////////////////////////////////////////////////////////////////
// Name:       WinDynamicLoader::FindFunction(const void* handle, const std::string& funcName)
// Purpose:    Implementation of WinDynamicLoader::FindFunction()
// Parameters:
// - handle
// - funcName
// Return:     void*
////////////////////////////////////////////////////////////////////////

void* WinDynamicLoader::FindFunction(const void* module, const std::string& funcName)
{
    return (void*)::GetProcAddress(HMODULE(module), funcName.data());
}

void * WinDynamicLoader::LoadModule(const std::string & modulePath)
{
	return ::LoadLibraryA(modulePath.data());
}

bool WinDynamicLoader::UnloadModule(const void * module)
{
	return ::FreeLibrary(HMODULE(module));
}

#endif