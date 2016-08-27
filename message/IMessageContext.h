/***********************************************************************
* Module:  IMessageContext.h
* Author:  milk
* Modified: 2016Äê10ÔÂ6ÈÕ 20:13:19
* Purpose: Declaration of the class IMessageContext
***********************************************************************/

#if !defined(__message_IMessageContext_h)
#define __message_IMessageContext_h

#include "../common/typedefs.h"

class IMessageContext
{
public:
	virtual attribute_ptr getAttribute(const std::string& key) = 0;
	virtual void setAttribute(const std::string& key, const attribute_ptr& value) = 0;
};

typedef std::shared_ptr<IMessageContext> IMessageContext_Ptr;

#endif