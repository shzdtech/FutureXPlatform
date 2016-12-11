/***********************************************************************
* Module:  CTSReturnTrade.h
* Author:  xiaoyu
* Modified: 2016Äê11ÔÂ8ÈÕ 22:26:40
* Purpose: Declaration of the class CTSReturnTrade
***********************************************************************/

#if !defined(__CTSServer_CTSReturnTrade_h)
#define __CTSServer_CTSReturnTrade_h

#include "../message/IMessageHandler.h"

class CTSReturnTrade : public IMessageHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif#pragma once
