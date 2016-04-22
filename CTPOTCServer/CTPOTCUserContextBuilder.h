/***********************************************************************
 * Module:  CTPOTCUserContextBuilder.h
 * Author:  milk
 * Modified: 2016年2月9日 10:24:06
 * Purpose: Declaration of the class CTPOTCUserContextBuilder
 ***********************************************************************/

#if !defined(__CTPOTCServer_CTPOTCUserContextBuilder_h)
#define __CTPOTCServer_CTPOTCUserContextBuilder_h

#include "../message/IUserContextBuilder.h"
#include "../utility/singleton_templ.h"
#include <map>

class CTPOTCUserContextBuilder : public IUserContextBuilder, public singleton_ptr < CTPOTCUserContextBuilder >
{
public:
	CTPOTCUserContextBuilder();
	~CTPOTCUserContextBuilder();

	void BuildContext(ISession* pSession);

protected:
	std::map<int, IUserContextBuilder_Ptr> _userContextMap;

private:

};

#endif