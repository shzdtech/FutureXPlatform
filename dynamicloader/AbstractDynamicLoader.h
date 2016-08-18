/***********************************************************************
 * Module:  AbstractDynamicLoader.h
 * Author:  milk
 * Modified: 2014年10月1日 12:13:03
 * Purpose: Declaration of the class AbstractDynamicLoader
 ***********************************************************************/

#if !defined(__dynamicloader_AbstractDynamicLoader_h)
#define __dynamicloader_AbstractDynamicLoader_h

#include <string>
#include <map>
#include "dynamicloader_exp.h"

class CLASS_EXPORT AbstractDynamicLoader
{
public:
	static AbstractDynamicLoader* Instance(void);
	virtual void* LoadModule(const char* moduleUUID, const char* modulePath);
	virtual void* FindModule(const char* moduleUUID);
	virtual bool UnloadModule(const char* moduleUUID);
	virtual bool CreateInstance(const void* handle, const char* classUUID, void** instance);

	virtual void* LoadModule(const char* modulePath) = 0;
	virtual bool UnloadModule(const void* module) = 0;
	virtual void* FindFunction(const void* module, const char* funcName) = 0;

protected:
	std::map<std::string, void*> uuidHandlerMap;
	static const char* STR_CREATEINSTANCE;
	AbstractDynamicLoader();

private:

};

#endif