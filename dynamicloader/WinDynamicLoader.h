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
	virtual void* FindFunction(const void* module, const std::string& funcName);
	virtual void* LoadModule(const std::string& modulePath);
	virtual bool UnloadModule(const void* module);

protected:
private:

};

#endif