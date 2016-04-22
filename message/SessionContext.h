/***********************************************************************
 * Module:  Context.h
 * Author:  milk
 * Modified: 2014年10月6日 20:36:20
 * Purpose: Declaration of the class Context
 ***********************************************************************/

#if !defined(__message_Context_h)
#define __message_Context_h

#include "ISessionContext.h"
#include <map>
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT SessionContext : public ISessionContext
{
public:
   Attrib_Ptr getAttribute(const std::string& key);
   void setAttribute(const std::string& key, Attrib_Ptr value);

protected:
private:
   std::map<std::string, Attrib_Ptr> _attrib_map;


};

#endif