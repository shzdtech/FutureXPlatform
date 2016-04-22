/***********************************************************************
 * Module:  IContext.h
 * Author:  milk
 * Modified: 2014年10月6日 20:13:19
 * Purpose: Declaration of the class IContext
 ***********************************************************************/

#if !defined(__message_IContext_h)
#define __message_IContext_h
#include "IUserInfo.h"

class ISessionContext
{
public:
	virtual Attrib_Ptr getAttribute(const std::string& key) = 0;
	virtual void setAttribute(const std::string& key, Attrib_Ptr value) = 0;

protected:
private:

};

typedef std::shared_ptr<ISessionContext> ISessionContext_Ptr;

#endif