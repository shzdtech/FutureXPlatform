/***********************************************************************
 * Module:  PBResultSerializer.cpp
 * Author:  milk
 * Modified: 2015年8月23日 21:57:11
 * Purpose: Implementation of the class PBResultSerializer
 ***********************************************************************/

#include "PBResultSerializer.h"
#include "../Protos/errormsg.pb.h"
#include "pbmacros.h"
#include "../dataobject/ResultDO.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       PBResultSerializer::Serialize(const dataobj_ptr& abstractDO)
 // Purpose:    Implementation of PBResultSerializer::Serialize()
 // Parameters:
 // - abstractDO
 // Return:     data_buffer
 ////////////////////////////////////////////////////////////////////////

data_buffer PBResultSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	Micro::Future::Message::Result PB;

	auto pDO = (ResultDO*)abstractDO.get();

	if (pDO->SerialId != 0)
		PB.set_serialid(pDO->SerialId);

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
	Micro::Future::Message::Result PB;
	ParseWithReturn(PB, rawdata)

	return std::make_shared<ResultDO>(PB.code(), PB.serialid());
}