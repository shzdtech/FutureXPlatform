#if !defined(__CTSServer_CTSQueryTrade_h)
#define __CTSServer_CTSQueryTrade_h

#include "../message/IMessageHandler.h"

class CTSQueryTrade : public IMessageHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif