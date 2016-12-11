/***********************************************************************
 * Module:  OTCUserContextBuilder.h
 * Author:  milk
 * Modified: 2016年2月9日 10:24:06
 * Purpose: Declaration of the class OTCUserContextBuilder
 ***********************************************************************/

#if !defined(__OTCServer_OTCUserContextBuilder_h)
#define __OTCServer_OTCUserContextBuilder_h

#include "../message/IUserContextBuilder.h"
#include "../utility/singleton_templ.h"
#include <map>
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCUserContextBuilder : public IUserContextBuilder, public singleton_ptr < OTCUserContextBuilder >
{
public:
	OTCUserContextBuilder();
	~OTCUserContextBuilder();

	void BuildContext(const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
	std::map<int, IUserContextBuilder_Ptr> _userContextMap;

private:

};

#endif