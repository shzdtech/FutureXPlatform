
/***********************************************************************
 * Module:  PBExceptionSerializer.cpp
 * Author:  milk
 * Modified: 2014年10月22日 11:25:39
 * Purpose: Implementation of the class PBExceptionSerializer
 ***********************************************************************/

#include "PBExceptionSerializer.h"
#include "pbmacros.h"
#include "../Protos/errormsg.pb.h"

using namespace Micro::Future::Message;

////////////////////////////////////////////////////////////////////////
// Name:       PBExceptionSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBExceptionSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBExceptionSerializer::Deserialize(const data_buffer& rawdata)
{
	ExceptionMessage exceptionMsg;
	ParseWithReturn(exceptionMsg, rawdata)

	auto ret = std::make_shared<MessageExceptionDO> (exceptionMsg.messageid(), exceptionMsg.serialid(),
		exceptionMsg.errortype(), exceptionMsg.errorcode(), exceptionMsg.description());

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       PBExceptionSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBExceptionSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBExceptionSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	auto pDO = (MessageExceptionDO*)abstractDO.get();
	ExceptionMessage exceptionMsg;
	if (pDO->SerialId != 0)
		exceptionMsg.set_serialid(pDO->SerialId);

	exceptionMsg.set_messageid(pDO->MessageId);

	exceptionMsg.set_errortype(pDO->ErrorType);

	if(pDO->ErrorCode)
		exceptionMsg.set_errorcode(pDO->ErrorCode);

	if(!pDO->ErrorMessage.empty())
		exceptionMsg.set_description(pDO->ErrorMessage);
	
	int bufsz = exceptionMsg.ByteSize();
	char* buf = new char[bufsz];
	exceptionMsg.SerializeToArray(buf, bufsz);

	return data_buffer(buf, bufsz);
}