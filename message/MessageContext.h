/***********************************************************************
 * Module:  MessageContext.h
 * Author:  milk
 * Modified: 2014年10月6日 20:36:20
 * Purpose: Declaration of the class MessageContext
 ***********************************************************************/

#if !defined(__message_MessageContext_h)
#define __message_MessageContext_h

#include "IMessageContext.h"
#include <map>
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT MessageContext : public IMessageContext
{
public:
   attribute_ptr getAttribute(const std::string& key);
   void setAttribute(const std::string& key, attribute_ptr value);

protected:

private:
   std::map<std::string, attribute_ptr> _attrib_map;
};

#endif