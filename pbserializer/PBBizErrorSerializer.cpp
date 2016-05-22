/***********************************************************************
 * Module:  PBBizErrorSerializer.cpp
 * Author:  milk
 * Modified: 2014年10月22日 11:25:39
 * Purpose: Implementation of the class PBBizErrorSerializer
 ***********************************************************************/

#include "../message/BizError.h"
#include "PBBizErrorSerializer.h"
#include "ExceptionDef.h"
#include "proto/errormsg.pb.h"

using namespace Micro::Future::Message::Business;

////////////////////////////////////////////////////////////////////////
// Name:       PBBizErrorSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBBizErrorSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBBizErrorSerializer::Deserialize(const data_buffer& rawdata)
{
	BizErrorMsg bizMsg;
	if(!bizMsg.ParseFromArray(rawdata.get(), rawdata.size()))
		throw BizError(INVALID_DATAFORMAT_CODE, INVALID_DATAFORMAT_DESC);

	auto ret = std::make_shared<BizErrorDO>
		(bizMsg.messageid(), bizMsg.errorcode(), bizMsg.description(), 
			bizMsg.syserrcode(), bizMsg.serialid());

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       PBBizErrorSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBBizErrorSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBBizErrorSerializer::Serialize(const dataobj_ptr abstractDO)
{
	auto bizErr = (BizErrorDO*)abstractDO.get();
	BizErrorMsg bizMsg;
	if (bizErr->SerialId != 0)
		bizMsg.set_serialid(bizErr->SerialId);

	bizMsg.set_messageid(bizErr->MessgeId);
	bizMsg.set_errorcode(bizErr->ErrorCode);
	bizMsg.set_description(bizErr->ErrorMessage);
	bizMsg.set_syserrcode(bizErr->SysErrCode);
	
	int bufsz = bizMsg.ByteSize();
	char* buf = new char[bufsz];
	bizMsg.SerializeToArray(buf, bufsz);

	return data_buffer(buf, bufsz);
}