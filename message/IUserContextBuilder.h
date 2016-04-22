/***********************************************************************
 * Module:  IUserContextBuilder.h
 * Author:  milk
 * Modified: 2016年2月8日 23:22:54
 * Purpose: Declaration of the class IUserContextBuilder
 ***********************************************************************/

#if !defined(__message_IUserContextBuilder_h)
#define __message_IUserContextBuilder_h

#include "ISession.h"
#include <memory>

class IUserContextBuilder
{
public:
	virtual void BuildContext(ISession* pSession) = 0;

protected:
private:

};

typedef std::shared_ptr<IUserContextBuilder> IUserContextBuilder_Ptr;

#endif