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
// Name:       WinDynamicLoader::FindFunction(const void* handle, const char* funcName)
// Purpose:    Implementation of WinDynamicLoader::FindFunction()
// Parameters:
// - handle
// - funcName
// Return:     void*
////////////////////////////////////////////////////////////////////////

void* WinDynamicLoader::FindFunction(const void* module, const char* funcName)
{
    return (void*)::GetProcAddress(HMODULE(module), funcName);
}

void * WinDynamicLoader::LoadModule(const char* modulePath)
{
	return ::LoadLibraryA(modulePath);
}

bool WinDynamicLoader::UnloadModule(const void * module)
{
	return ::FreeLibrary(HMODULE(module));
}

#endif