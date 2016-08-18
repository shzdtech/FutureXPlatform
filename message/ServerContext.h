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
   attribute_ptr getAttribute(const std::string& key);
   void setAttribute(const std::string& key, attribute_ptr value);
   bool getConfigVal(const std::string& cfgKey, std::string& cfgVal);
   void setConfigVal(const std::string& cfgKey, const std::string& cfgVal);
   int getServerType(void);
   void setServerType(int type);
   void setWorkerProcessor(const IMessageProcessor_Ptr& proc_ptr);
   IMessageProcessor_Ptr getWorkerProcessor(void);
   void setSubTypeWorkerPtr(const shared_void_ptr& proc_ptr);
   shared_void_ptr getSubTypeWorkerPtr(void);

protected:

private:
   std::map<std::string, attribute_ptr> _attrib_map;
   std::map<std::string, std::string> _configs;
   IMessageProcessor_Ptr _workerProcessor_Ptr;
   shared_void_ptr _subWorkProcPtr;
   int _serverType = 0;
};

#endif