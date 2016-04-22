/***********************************************************************
 * Module:  WinDynamicLoader.cpp
 * Author:  milk
 * Modified: 2014年10月1日 15:52:43
 * Purpose: Implementation of the class WinDynamicLoader
 ***********************************************************************/

#include "WinDynamicLoader.h"
#include <windows.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////
// Name:       WinDynamicLoader::LoadModule(const std::string& moduleUUID, const std::string& modulePath)
// Purpose:    Implementation of WinDynamicLoader::LoadModule()
// Parameters:
// - moduleUUID
// - modulePath
// Return:     void*
////////////////////////////////////////////////////////////////////////

void* WinDynamicLoader::LoadModule(const std::string& moduleUUID, const std::string& modulePath)
{
    HMODULE hModel = NULL;
    auto itr = uuidHandlerMap.find(moduleUUID);
    if (itr != uuidHandlerMap.end()) {
        hModel = HMODULE(itr->second);
    } else {
        hModel = ::LoadLibraryA(modulePath.data());
        if (hModel)
            uuidHandlerMap[moduleUUID] = hModel;
        else
        {
            DWORD error = ::GetLastError();
            std::cout<<"Load lib failed: "<<error<<std::endl;
        }
    }
    
    return hModel;
}

////////////////////////////////////////////////////////////////////////
// Name:       WinDynamicLoader::CreateInstance(void* handle, std::string classUUID, void** instance)
// Purpose:    Implementation of WinDynamicLoader::CreateInstance()
// Parameters:
// - handle
// - classUUID
// - instance
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool WinDynamicLoader::CreateInstance(const void* handle, const std::string& classUUID, void** instance)
{
    *instance = NULL;
    typedef void* (*instanceFun)(const char*);
    instanceFun instanceCall; 
    instanceCall = (instanceFun)FindFunction(handle, AbstractDynamicLoader::STR_CREATEINSTANCE);
    if (instanceCall != NULL)
        *instance = instanceCall(classUUID.data());
    
    return *instance != NULL;
}

////////////////////////////////////////////////////////////////////////
// Name:       WinDynamicLoader::FindModule(const std::string& moduleUUID)
// Purpose:    Implementation of WinDynamicLoader::FindModule()
// Parameters:
// - moduleUUID
// Return:     void*
////////////////////////////////////////////////////////////////////////

void* WinDynamicLoader::FindModule(const std::string& moduleUUID)
{
    void* funcPtr = NULL;
    auto iter = uuidHandlerMap.find(moduleUUID); 
    if( iter != uuidHandlerMap.end() )
        funcPtr = iter->second;
    
    return funcPtr;
}

////////////////////////////////////////////////////////////////////////
// Name:       WinDynamicLoader::FindFunction(const void* handle, const std::string& funcName)
// Purpose:    Implementation of WinDynamicLoader::FindFunction()
// Parameters:
// - handle
// - funcName
// Return:     void*
////////////////////////////////////////////////////////////////////////

void* WinDynamicLoader::FindFunction(const void* handle, const std::string& funcName)
{
    FARPROC funPtr = ::GetProcAddress(HMODULE(handle), funcName.data());
    return (void*)funPtr;
}

////////////////////////////////////////////////////////////////////////
// Name:       WinDynamicLoader::UnloadModule(const std::string& moduleUUID)
// Purpose:    Implementation of WinDynamicLoader::UnloadModule()
// Parameters:
// - moduleUUID
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool WinDynamicLoader::UnloadModule(const std::string& moduleUUID)
{
    bool ret = true;
    
    auto iter = uuidHandlerMap.find(moduleUUID); 
    if( iter != uuidHandlerMap.end() )
    {
        // Thread safe
        if ( uuidHandlerMap.erase(iter->first) > 0 )
            ret = ::FreeLibrary(HMODULE(iter->second));
    }
    
    return ret;
}