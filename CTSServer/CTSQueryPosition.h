/***********************************************************************
* Module:  CTSQueryPosition.h
* Author:  xiaoyu
* Modified: 2016��11��25�� 12:57:18
* Purpose: Declaration of the class CTSQueryPosition
***********************************************************************/

#if !defined(__CTSServer_CTSQueryPosition_h)
#define __CTSServer_CTSQueryPosition_h

#include "../message/IMessageHandler.h"

class CTSQueryPosition : public IMessageHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif