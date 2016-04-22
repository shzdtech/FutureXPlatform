/***********************************************************************
 * Module:  PBResultSerializer.cpp
 * Author:  milk
 * Modified: 2015年8月23日 21:57:11
 * Purpose: Implementation of the class PBResultSerializer
 ***********************************************************************/

#include "PBResultSerializer.h"
#include "proto/errormsg.pb.h"
#include "../message/BizError.h"
#include "ExceptionDef.h"
#include "../dataobject/ResultDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBResultSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBResultSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBResultSerializer::Serialize(const dataobj_ptr abstractDO)
{
	Micro::Future::Message::Business::Result PB;

	auto pDO = (ResultDO*)abstractDO.get();

	PB.set_code(pDO->Code);

	int bufsize = PB.ByteSize();
	uint8_t* buf = new uint8_t[bufsize];
	PB.SerializeToArray(buf, bufsize);

	return data_buffer(buf, bufsize);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBResultSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBResultSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBResultSerializer::Deserialize(const data_buffer& rawdata)
{
	Micro::Future::Message::Business::Result PB;

	auto ret = std::make_shared<ResultDO>();

	if (!PB.ParseFromArray(rawdata.get(), rawdata.size()))
		throw BizError(INVALID_DATAFORMAT_CODE, INVALID_DATAFORMAT_DESC);

	ret->Code = PB.code();

	return ret;
}