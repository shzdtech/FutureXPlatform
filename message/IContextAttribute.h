/***********************************************************************
 * Module:  IContextAttribute.h
 * Author:  milk
 * Modified: 2014年10月6日 20:13:19
 * Purpose: Declaration of the class IContextAttribute
 ***********************************************************************/

#if !defined(__message_IContextAttribute_h)
#define __message_IContextAttribute_h

#include "../common/typedefs.h"

class IContextAttribute
{
public:
	virtual attribute_ptr getAttribute(const std::string& key) = 0;
	virtual void setAttribute(const std::string& key, attribute_ptr value) = 0;

protected:
private:

};

typedef std::shared_ptr<IContextAttribute> IContextAttribute_Ptr;

#endif