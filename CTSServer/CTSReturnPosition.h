/***********************************************************************
* Module:  CTSReturnPosition.h
* Author:  xiaoyu
* Modified: 2016Äê11ÔÂ8ÈÕ 22:26:40
* Purpose: Declaration of the class CTSReturnPosition
***********************************************************************/

#if !defined(__CTSServer_CTSReturnPosition_h)
#define __CTSServer_CTSReturnPosition_h

#include "../message/IMessageHandler.h"

class CTSReturnPosition : public IMessageHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif#pragma once
#pragma once
