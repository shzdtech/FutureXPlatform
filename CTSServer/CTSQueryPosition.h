/***********************************************************************
* Module:  CTSQueryPosition.h
* Author:  xiaoyu
* Modified: 2016Äê11ÔÂ25ÈÕ 12:57:18
* Purpose: Declaration of the class CTSQueryPosition
***********************************************************************/

#if !defined(__CTSServer_CTSQueryPosition_h)
#define __CTSServer_CTSQueryPosition_h

#include "../message/IMessageHandler.h"

class CTSQueryPosition : public IMessageHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif