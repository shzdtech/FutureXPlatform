/***********************************************************************
 * Module:  SODynamicLoader.h
 * Author:  milk
 * Modified: 2016年5月4日 1:13:16
 * Purpose: Declaration of the class SODynamicLoader
 ***********************************************************************/

#if !defined(__dynamicloader_SODynamicLoader_h)
#define __dynamicloader_SODynamicLoader_h

#include "AbstractDynamicLoader.h"

class SODynamicLoader : public AbstractDynamicLoader
{
public:
   void* LoadModule(const std::string& modulePath);
   bool UnloadModule(const void* module);
   void* FindFunction(const void* module, const std::string& funcName);

protected:
private:

};

#endif