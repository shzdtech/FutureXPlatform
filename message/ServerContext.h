/***********************************************************************
 * Module:  Context.h
 * Author:  milk
 * Modified: 2014年10月6日 20:36:20
 * Purpose: Declaration of the class Context
 ***********************************************************************/

#if !defined(__message_ServerContext_h)
#define __message_ServerContext_h

#include "IServerContext.h"
#include <map>
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT ServerContext : public IServerContext
{
public:
	ServerContext();

	attribute_ptr getAttribute(const std::string& key);
	void setAttribute(const std::string& key, attribute_ptr value);
	bool getConfigVal(const std::string& cfgKey, std::string& cfgVal);
	void setConfigVal(const std::string& cfgKey, const std::string& cfgVal);
	int getServerType(void);
	void setServerType(int type);
	void setWorkerProcessor(const IMessageProcessor_Ptr& proc_ptr);
	IMessageProcessor_Ptr getWorkerProcessor(void);
	void setSubTypeWorkerPtr(void* pProcess);
	void* getSubTypeWorkerPtr(void);
	const std::string& getServerUri(void);
	void setServerUri(const std::string& serverUri);
	void Reset(void);

protected:

private:
	std::string _serverUri;
	std::map<std::string, attribute_ptr> _attrib_map;
	std::map<std::string, std::string> _configs;
	IMessageProcessor_Ptr _workerProcessor_Ptr;
	void* _pSubWorkProc;
	int _serverType = 0;
};

#endif