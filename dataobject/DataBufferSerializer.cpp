/***********************************************************************
 * Module:  DataBufferSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:53:06
 * Purpose: Implementation of the class DataBufferSerializer
 ***********************************************************************/

#include "DataBufferSerializer.h"
#include "TemplateDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       DataBufferSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of DataBufferSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer DataBufferSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	auto databufDO = (DataBufferDO*)abstractDO.get();
	return databufDO->Data;
}

////////////////////////////////////////////////////////////////////////
// Name:       DataBufferSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of DataBufferSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr DataBufferSerializer::Deserialize(const data_buffer& rawdata)
{
	size_t bufSz = rawdata.size();
	return dataobj_ptr(new DataBufferDO(rawdata));
}