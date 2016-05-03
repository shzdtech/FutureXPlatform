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
	virtual void* LoadModule(const std::string& moduleUUID, const std::string& modulePath);
	virtual void* FindModule(const std::string& moduleUUID);
	virtual bool UnloadModule(const std::string& moduleUUID);
	virtual bool CreateInstance(const void* handle, const std::string& classUUID, void** instance);

	virtual void* LoadModule(const std::string& modulePath) = 0;
	virtual bool UnloadModule(const void* module) = 0;
	virtual void* FindFunction(const void* module, const std::string& funcName) = 0;

protected:
	std::map<std::string, void*> uuidHandlerMap;
	static std::string STR_CREATEINSTANCE;
	AbstractDynamicLoader();

private:

};

#endif