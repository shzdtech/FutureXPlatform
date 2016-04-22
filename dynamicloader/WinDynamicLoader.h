/***********************************************************************
 * Module:  WinDynamicLoader.h
 * Author:  milk
 * Modified: 2014年10月1日 15:52:43
 * Purpose: Declaration of the class WinDynamicLoader
 ***********************************************************************/

#if !defined(__dynamicloader_WinDynamicLoader_h)
#define __dynamicloader_WinDynamicLoader_h

#include "AbstractDynamicLoader.h"

class WinDynamicLoader : public AbstractDynamicLoader
{
public:
   void* LoadModule(const std::string& moduleUUID, const std::string& modulePath);
   bool CreateInstance(const void* handle, const std::string& classUUID, void** instance);
   void* FindModule(const std::string& moduleUUID);
   void* FindFunction(const void* handle, const std::string& funcName);
   bool UnloadModule(const std::string& moduleUUID);

protected:
private:

};

#endif